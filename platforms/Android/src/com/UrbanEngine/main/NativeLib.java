package com.UrbanEngine.main;

import android.widget.Toast;
import android.content.Context;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;


public class NativeLib
{
	public static Context contect;
	static
	{
        System.loadLibrary("Filter");
    }
	
	//logic
	public static native void AndroidLogicCreate();
    public static native void AndroidLogicUpdate();
	
	
	//graphic
	public static native void AndroidGraphicInit();
	public static native void AndroidGraphicRender();
    public static native void AndroidGraphicSetSize(int width, int height);
	
	
	//input
    public static native void on_touch_press(float x, float y);
	
	
	
	//printText
	public static void printText(byte[] text) 
	{
		Toast.makeText(contect, new String(text, StandardCharsets.UTF_8), Toast.LENGTH_SHORT).show();
    }

	//readFile
	public static byte[] readFile(byte[] filename)
	{
		//Toast.makeText(contect, new String(filename, StandardCharsets.UTF_8), Toast.LENGTH_SHORT).show();
		String filenm = new String(filename, StandardCharsets.UTF_8);
		byte buffer[];
		try{
			InputStream is = contect.getResources().getAssets().open(filenm);
			int size = is.available();
			buffer = new byte[size + 5];
			is.read(buffer, 4, size);
			is.close();
			
			buffer[0] = (byte) (size >> 24);
			buffer[1] = (byte) (size >> 16);
			buffer[2] = (byte) (size >> 8);
			buffer[3] = (byte) (size     );
			buffer[size + 4] = (byte)0;
			//Toast.makeText(contect, String.valueOf(size), Toast.LENGTH_SHORT).show();
		}
		catch(IOException io) 
		{
			buffer = new byte[1];
		}
		//Toast.makeText(contect, new String(buffer, StandardCharsets.UTF_8), Toast.LENGTH_SHORT).show();
		
		return buffer;
	}	
};