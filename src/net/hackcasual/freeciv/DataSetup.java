/********************************************************************** 
 Android-Freeciv - Copyright (C) 2010 - C Vaughn
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

package net.hackcasual.freeciv;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import android.util.Log;

public class DataSetup {
	public static void doIt(InputStream freeCivZip) {
		if ((new File("/data/data/net.hackcasual.freeciv/freeciv")).exists()) return;
		
		byte[] buffer = new byte[1024];
		
		ZipInputStream zf = new ZipInputStream(freeCivZip);
		try {
			ZipEntry ze;
			while ((ze = zf.getNextEntry()) != null) {
				Log.d("SpankyTheThunderball", ze.getName());
				
				if (!ze.isDirectory()) {
					
					
					
					File dest = new File(String.format("/data/data/net.hackcasual.freeciv/%s", ze.getName()));
					
					File destDir = dest.getParentFile();
					
					if (!destDir.exists()) {
						destDir.mkdirs();
					}
					
					FileOutputStream fos = new FileOutputStream(dest);
					
					int readCount;
					while ((readCount = zf.read(buffer)) >= 0) {
						fos.write(buffer,0,readCount);
					}
					
					fos.close();
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
