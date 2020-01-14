#include "qr_code_dialog.hpp"
#include "settings.hpp"
#include "resource.h"

#include "../log.hpp"
#include "../system.hpp"
#include "../string_utils.hpp"

#include <string>
#include <vector>
#include <qrcodegen/QrCode.hpp>
#include <boost/asio/ip/host_name.hpp>

using namespace qrcodegen;

namespace msrv {
namespace player_foobar2000 {

	QrCodeDialog* QrCodeDialog::current_ = nullptr;

	QrCodeDialog::QrCodeDialog()
		: handle_(nullptr),
		  image_(nullptr)
	{
		current_ = this;
	}

	QrCodeDialog::~QrCodeDialog()
	{
		current_ = nullptr;
	}

	void QrCodeDialog::show()
	{
		tryCatchLog([](){
			if (current_ && current_->handle_)
			{
				SetForegroundWindow(current_->handle_);
			}
			else
			{
				pfc::refcounted_object_ptr_t<QrCodeDialog> instance;
				instance.copy(new QrCodeDialog());
				instance->create();
			}
		});
	}

	void QrCodeDialog::create()
	{
		auto ret = CreateDialogParamW(
			core_api::get_my_instance(),
			MAKEINTRESOURCEW(IDD_QRCODE),
			core_api::get_main_window(),
			&dialogProcWrapper,
			reinterpret_cast<LPARAM>(this));

		throwIfFailed("CreateDialogParamW", ret != nullptr);
	}

	INT_PTR CALLBACK QrCodeDialog::dialogProcWrapper(
		HWND window, UINT message, WPARAM wparam, LPARAM lparam)
	{
		pfc::refcounted_object_ptr_t<QrCodeDialog> instance;

		if (message == WM_INITDIALOG)
		{
			instance.copy(reinterpret_cast<QrCodeDialog*>(lparam));
			instance->handle_ = window;
			instance->refcount_add_ref();
			SetWindowLongPtrW(window, DWLP_USER, lparam);
			modeless_dialog_manager::g_add(window);
		}
		else
		{
			instance.copy(reinterpret_cast<QrCodeDialog*>(GetWindowLongPtrW(window, DWLP_USER)));
		}

		INT_PTR result = 0;

		if (instance.is_valid())
		{
			bool processed = tryCatchLog([&]
			{
				result = instance->dialogProc(message, wparam, lparam);
			});
		}

		if (message == WM_DESTROY)
		{
			modeless_dialog_manager::g_remove(window);

			if (instance.is_valid() && instance->handle_)
			{
				instance->handle_ = nullptr;
				instance->refcount_release();
			}
		}

		return result;
	}

	INT_PTR QrCodeDialog::dialogProc(UINT message, WPARAM wparam, LPARAM lparam)
	{
		switch (message)
		{
		case WM_INITDIALOG:
			initialize();
			return 1;

		case WM_COMMAND:
			return handleCommand(LOWORD(wparam), HIWORD(wparam));

		case WM_DESTROY:
			destroy();
			return 0;
		}

		return 0;
	}

	void QrCodeDialog::initialize()
	{
		std::string host = boost::asio::ip::host_name();
		int port = SettingVars::port.get_value();
		pfc::string_formatter url;
		url << "http://" << host.c_str() << ":" << port << "/";

		QrCode qr = QrCode::encodeText(url.c_str(), QrCode::Ecc::MEDIUM);
		int size = qr.getSize();

		HDC windowDC = GetWindowDC(NULL);
		HDC imageDC = CreateCompatibleDC(windowDC);

		int scale = 8;
		int margin = 4;
		image_ = CreateCompatibleBitmap(windowDC, (margin + size + margin) * scale, (margin + size + margin) * scale);
		{
			SelectObjectScope bitmapScope = {imageDC, image_};
			HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
			HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
		
			RECT rect = RECT{
				0, 0,
				(margin + size + margin) * scale, (margin + size + margin) * scale
			};
			FillRect(imageDC, &rect, whiteBrush);

			for (int y = 0; y < size; ++y)
			{
				for (int x = 0; x < size; ++x)
				{
					if (qr.getModule(x, y))
					{
						RECT moduleRect = RECT{
							(margin + x) * scale, (margin + y) * scale,
							(margin + x +1 ) * scale, (margin + y + 1) * scale
						};
						FillRect(imageDC, &moduleRect, blackBrush);
					}
				}
			}

			DeleteObject(blackBrush);
			DeleteObject(whiteBrush);
		}

		DeleteDC(imageDC);
		ReleaseDC(handle_, windowDC);

		SendDlgItemMessageW(handle_, IDC_QRCODE, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) image_);
	}

	void QrCodeDialog::destroy()
	{
		if (image_)
		{
			DeleteObject(image_);
			image_ = nullptr;
		}
	}

	INT_PTR QrCodeDialog::handleCommand(int control, int message)
	{
		switch (control)
		{
		case IDCANCEL:
		case IDCLOSE:
			DestroyWindow(handle_);
			return 1;
		}
		return 0;
	}

	namespace {

		// {72358E70-D7EB-43E2-AA0A-F6347A8AE820}
		static const GUID command_guid_ =
		{ 0x72358e70, 0xd7eb, 0x43e2,{ 0xaa, 0xa, 0xf6, 0x34, 0x7a, 0x8a, 0xe8, 0x20 } };

		class MainMenuCommands : public mainmenu_commands
		{
		public:
			t_uint32 get_command_count() override
			{
				return 1;
			}

			GUID get_command(t_uint32 p_index) override
			{
				return command_guid_;
			}

			void get_name(t_uint32 p_index, pfc::string_base & p_out) override
			{
				p_out = MSRV_PROJECT_NAME;
			}

			bool get_description(t_uint32 p_index, pfc::string_base & p_out) override
			{
				return false;
			}

			GUID get_parent() override
			{
				return mainmenu_groups::view;
			}

			void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) override
			{
				QrCodeDialog::show();
			}
		};

		mainmenu_commands_factory_t<MainMenuCommands> MainMenuCommandsFactory;
	}

}}