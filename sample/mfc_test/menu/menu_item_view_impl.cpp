#include "stdafx.h"
#include "menu_item_view_impl.h"
#include "ui\gfx\canvas.h"
#include "SkPaint.h"
#include "SkCanvas.h"

void TestMenuItemView::OnPaint(gfx::Canvas* canvas) {
	//canvas->DrawStringInt(L"A sample item");
	SkColor color1 = SkColorSetRGB(255, 255, 255);
	canvas->FillRect(color1, gfx::Rect(2, 2, width() - 4 , height() - 4));

	TRACE(L"%d %d %d %d\n", x(), y(), width(), height());
	SkColor color = SkColorSetRGB(255, 0, 0);
	//canvas->DrawLineInt(color, 0, 0, 200, 20);
	SkPaint paint;
	paint.setAntiAlias(true);
	paint.setColor(SkColorSetRGB(255, 0, 0));
	paint.setStrokeWidth(3);
	paint.setStrokeCap(SkPaint::kRound_Cap);
	canvas->GetSkCanvas()->drawLine(10, 10, 200, 20, paint);

}

gfx::Size TestMenuItemView::GetPreferredSize() {
	return gfx::Size(200, 30);
}
