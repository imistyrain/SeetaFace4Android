package yanyu.com.seetaface4android;

import android.content.Context;
import android.hardware.Camera;
import android.util.AttributeSet;

import org.opencv.android.JavaCameraView;

import java.util.List;

/**
 * Created by yanyu on 2016/10/13.
 */
public class MRCameraBridgeView extends JavaCameraView {
    private static final String TAG = "MRCameraBridgeView";
    public  MRCameraBridgeView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mMaxHeight=480;
        mMaxWidth=640;
    }
    public List<Camera.Size> getResolutionList() {
        return mCamera.getParameters().getSupportedPreviewSizes();
    }

    public void setResolution(int width,int height) {
        disconnectCamera();
        mMaxHeight = height;
        mMaxWidth = width;
        connectCamera(getWidth(), getHeight());
    }
    public void setResolution(Camera.Size resolution) {
        disconnectCamera();
        mMaxHeight = resolution.height;
        mMaxWidth = resolution.width;
        connectCamera(getWidth(), getHeight());
    }
    public Camera.Size getResolution() {
        return mCamera.getParameters().getPreviewSize();
    }
}
