package com.UrbanEngine.main;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;
import android.app.ActivityManager;
import android.content.Context;


import android.view.View;
import android.view.MotionEvent;

import android.opengl.GLSurfaceView;

public class MainActivity extends Activity {
	
	private GLSurfaceView glSurfaceView;
	private boolean rendererSet;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
		//check open gl es 3 support
		ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
		final boolean supportsEs3 = activityManager.getDeviceConfigurationInfo().reqGlEsVersion >= 0x30000;
		if (!supportsEs3) 
		{
			Toast.makeText(this, "This device does not support OpenGL ES 3.0.", Toast.LENGTH_LONG).show();
			return;
		} 
		
		
		
		RendererWrapper rend = new RendererWrapper();
		NativeLib.contect = this;
		//NativeLib.AndroidGraphicInit();
		
		rendererSet = true;
		glSurfaceView = new GLSurfaceView(this);
		
		glSurfaceView.setOnTouchListener(
		new View.OnTouchListener() 
		{
			float x;
			float y;
			@Override
			public boolean onTouch(View v, MotionEvent event) 
			{
				if(event == null)
				{
					return false;
				} 
				x = (event.getX() / (float)v.getWidth ()) * 2 - 1;
				y = (event.getY() / (float)v.getHeight()) * 2 - 1;
				
				if(event.getAction() == MotionEvent.ACTION_DOWN) {
					glSurfaceView.queueEvent(new Runnable()  {
						@Override
						public void run() {
							NativeLib.on_touch_press(x, y);
						}
					});
				}
				
				return true;
			}
		});
		
		glSurfaceView.setEGLContextClientVersion(3);
		glSurfaceView.setRenderer(rend);
		setContentView(glSurfaceView);
		
		//setContentView(R.layout.main);
    }
	
	@Override
	protected void onPause() {
		super.onPause();
	 
		if (rendererSet) {
			glSurfaceView.onPause();
		}
	}
	 
	@Override
	protected void onResume() {
		super.onResume();
	 
		if (rendererSet) {
			glSurfaceView.onResume();
		}
	}
}