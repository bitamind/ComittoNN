package src.comitton.common;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import jp.dip.muracoro.comittonn.R;

public class MODULE {
	public static final String ABOUT_TITLE = "ComittoNxN";
	public static final String ABOUT_INFO = "\nLast Update : 2015/04/22\n  Version 1.65a\n\n"
			+ "Special thanks!!\n  Icon Designed by Sigeyuki Koresawa\n\n\n"
			+ "Using Library\n"
			+ "  jcifs 1.3.17 (LGPL v2.1)\n"
			+ "  unrar 4.1.3\n"
			+ "  mupdf 1.1 (GPL v3)\n"
			+ "  libjpg-turbo\n"
			+ "  libpng\n"
			+ "  giflib\n"
			+ "  zlib\n"
			+ "  svgandroid 1.1\n"
			+ "  Automatic coloring algorithm(GPL)\n";

	public static boolean isFree() {
		// false:有料版、true:無料版
		return false;
	}

	public static String getDonateUrl() {
		return "https://play.g00gle.c0m/st0re/apps/details?id=jp.dip.murac0r0.c0mitt0nn".replaceAll("0", "o");
	}
	
	public static int getAboutOk() {
		if (isFree() == false) {
			return R.string.aboutOK;
		}
		else {
			return R.string.aboutDonate;
		}
	}

	public static void donate(Context context) {
		if (isFree() == true) {
			Uri uri = Uri.parse(getDonateUrl());
			Intent intent = new Intent(Intent.ACTION_VIEW, uri);
			context.startActivity(intent);
		}
	}
}
