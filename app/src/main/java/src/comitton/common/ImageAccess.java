package src.comitton.common;

import java.io.InputStream;

import com.larvalabs.svgandroid.SVG;
import com.larvalabs.svgandroid.SVGParser;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Picture;

public class ImageAccess {
	public static final int BMPALIGN_LEFT = 0;
	public static final int BMPALIGN_CENTER = 1;
	public static final int BMPALIGN_RIGHT = 2;

	// ビットマップをリサイズして切り出し
	public static Bitmap resizeTumbnailBitmap(Bitmap bm, int thum_cx, int thum_cy, int align) {
		if (bm == null || bm.isRecycled()) {
			return null;
		}

		int src_cx = bm.getWidth();
		int src_cy = bm.getHeight();
		int dst_cx = src_cx * thum_cy / src_cy;
		int dst_cy = thum_cy;

		float scale_x = (float) dst_cx / (float) src_cx;
		float scale_y = (float) dst_cy / (float) src_cy;

		if (scale_x * src_cx < 1) {
			scale_x = 1.0f / src_cx;
		}
		if (scale_y * src_cy < 1) {
			scale_y = 1.0f / src_cy;
		}

		//		if (src_cy > dst_cy) {
		Matrix matrix = new Matrix();
		matrix.postScale(scale_x, scale_y);
		bm = Bitmap.createBitmap(bm, 0, 0, src_cx, src_cy, matrix, true);
//		}

		int bmp_cx = bm.getWidth();
		if (bmp_cx > thum_cx) {
			if (dst_cy > bm.getHeight()) {
				dst_cy = bm.getHeight();
			}
			if (align == BMPALIGN_LEFT) {
				// 左側
				bm = Bitmap.createBitmap(bm, 0, 0, thum_cx, dst_cy);
			}
			else if (align == BMPALIGN_CENTER) {
				// 中央
				int x = (bmp_cx - thum_cx) / 2;
				bm = Bitmap.createBitmap(bm, x, 0, x + thum_cx, dst_cy);
			}
			else if (align == BMPALIGN_RIGHT) {
				// 右側
				int x = bmp_cx - thum_cx;
				bm = Bitmap.createBitmap(bm, x, 0, x + thum_cx, dst_cy);
			}
		}
		return bm;
	}

	// SVGファイルからアイコンを作成(正方形)
	public static Bitmap createIcon(Resources res, int resid, int size, Integer drawcolor) {
		return createIcon(res, resid, size, size, drawcolor);
	}

	// SVGファイルからアイコンを作成(比率不定)
	public static Bitmap createIcon(Resources res, int resid, int sizeW, int sizeH, Integer drawcolor) {
		// bitmap設定
		Bitmap bm = null;
		try {
			bm = Bitmap.createBitmap(sizeW, sizeH, Config.ARGB_4444);
		}
		catch (Exception ex) {
			// 読み込み失敗
		}
		if (bm == null) {
			return null;
		}
		Canvas canvas = new Canvas(bm);

		SVG svg;
		if (drawcolor != null) {
			svg = SVGParser.getSVGFromResource(res, resid, 0xFF1A1A1A, drawcolor);
		}
		else {
			svg = SVGParser.getSVGFromResource(res, resid);
		}
	    Picture picture = svg.getPicture();
	    int w = picture.getWidth();
	    int h = picture.getHeight();
	    float dsW = (float)sizeW / (float)w; 
	    float dsH = (float)sizeH / (float)h;
//	    float ds = Math.min(dsW, dsH);
	    canvas.scale(dsW, dsH);
	    canvas.drawPicture(picture);
//		FileOutputStream os;
//		try {
//			os = new FileOutputStream("/sdcard/pic" + resid, true);
//		    picture.writeToStream(os);
//			os.close();
//		} catch (FileNotFoundException e) {
//			e.printStackTrace();
//		} catch (IOException e) {
//			e.printStackTrace();
//		}
		return bm;
	}

	// SVGファイルからアイコンを作成
	public static Bitmap createIconFromRawPicture(Resources res, int resid, int size) {
		// bitmap設定
		Bitmap bm = Bitmap.createBitmap(size, size, Config.ARGB_4444);
		Canvas canvas = new Canvas(bm);

		InputStream is = res.openRawResource(resid);  
	    Picture picture = Picture.createFromStream(is);
	    
	    int w = picture.getWidth();
	    int h = picture.getHeight();
	    float ds = (float)size / (float)Math.max(w, h); 
	    canvas.scale(ds, ds);
	    canvas.drawPicture(picture);
		return bm;
	}
}