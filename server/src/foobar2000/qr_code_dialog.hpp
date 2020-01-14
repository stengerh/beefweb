#pragma once

#include "common.hpp"
#include "../project_info.hpp"

namespace msrv {
namespace player_foobar2000 {

	class QrCodeDialog : public pfc::refcounted_object_root
	{
	public:
		QrCodeDialog();
		~QrCodeDialog();

		static void show();

	private:
		void create();

		static INT_PTR CALLBACK dialogProcWrapper(
			HWND window, UINT message, WPARAM wparam, LPARAM lparam);

		INT_PTR dialogProc(UINT message, WPARAM wparam, LPARAM lparam);
		INT_PTR handleCommand(int control, int message);

		void initialize();
		void destroy();

		HWND handle_;
		HBITMAP image_;

		// weak reference to current instance
		static QrCodeDialog* current_;
	};

}}
