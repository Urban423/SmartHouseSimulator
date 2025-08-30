package com.UrbanEngine.main;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLES20;


public class RendererWrapper implements Renderer
{
	
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		NativeLib.AndroidGraphicInit();
    }
 
    public void onSurfaceChanged(GL10 gl, int width, int height) {
		NativeLib.AndroidGraphicSetSize(width, height);
    }
 
    public void onDrawFrame(GL10 gl) {
		NativeLib.AndroidGraphicRender();
    }	
};
