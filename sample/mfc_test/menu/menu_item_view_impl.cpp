#include "stdafx.h"
#include "menu_item_view_impl.h"
#include "ui\gfx\canvas.h"
#include "SkPaint.h"
#include "SkCanvas.h"
#include "ui\base\resource\resource_bundle.h"

static const gfx::Font& GetFont() {
    ResourceBundle& rb = ResourceBundle::GetSharedInstance();
    return rb.GetFont(ResourceBundle::BaseFont);
}

void TestMenuItemView::OnPaint(gfx::Canvas* canvas) {
	//canvas->DrawStringInt(L"A sample item");
	SkColor color1 = SkColorSetRGB(255, 255, 255);
	canvas->FillRect(color1, gfx::Rect(2, 2, width() - 4 , height() - 4));

	//TRACE(L"%d %d %d %d\n", x(), y(), width(), height());
	SkColor color = SkColorSetRGB(255, 0, 0);
	//canvas->DrawLineInt(color, 0, 0, 200, 20);
	SkPaint paint;
	paint.setAntiAlias(true);
	paint.setColor(SkColorSetRGB(255, 0, 0));
	paint.setStrokeWidth(3);
	paint.setStrokeCap(SkPaint::kRound_Cap);
	canvas->GetSkCanvas()->drawLine(10, 10, 200, 20, paint);

    if (label_.GetLength()) {
        //HDC hdc = canvas->BeginPlatformPaint();
        //CRect text_rect(20, 5, 170, 25);
        //DrawText(hdc, label_, -1, text_rect, DT_SINGLELINE | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS );       
        //canvas->EndPlatformPaint();

        canvas->DrawStringInt(label_.GetBuffer(), GetFont(), SK_ColorBLACK, 20, 5, 150, 20);
    }

    if (IsSelected()) {
        canvas->FillRect(SkColorSetRGB(255, 0, 255), gfx::Rect(5, 5, 15, 15));
    }

    if (HasSubmenu()) {
        canvas->FillRect(SkColorSetRGB(255, 255, 0), gfx::Rect(185, 5, 15, 15));
    }
}

gfx::Size TestMenuItemView::GetPreferredSize() {
	return gfx::Size(200, 30);
}
