package net.hackcasual.freeciv.i18n;

public class TranslateStrings {
	public static String translate(String str) {
		if (str.startsWith("?"))
			return str.replaceFirst("^?[^:]*:", "");
		
		return str;
	}
}
