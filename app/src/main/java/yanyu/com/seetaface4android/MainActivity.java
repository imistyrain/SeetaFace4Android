package yanyu.com.seetaface4android;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.WindowManager;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.Mat;

public class MainActivity extends Activity implements CameraBridgeViewBase.CvCameraViewListener2{
    private static final String TAG = "SeetaFace";

    private Mat mRgba;
    private Mat mGray;
    private MRCameraBridgeView mOpenCvCameraView=null;
    private boolean bseetalibinited=false;
    private int cameindex = 1;
    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    initFile();
                    Log.i(TAG, "OpenCV loaded successfully");
                    try{
                        System.loadLibrary("seetaface_jni");
                        Log.i(TAG, "SeetaFace loaded successfully");
                        bseetalibinited=true;
                    }
                    catch (Throwable e){
                        e.printStackTrace();
                    }
                    mOpenCvCameraView.enableView();
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (checkSelfPermission(Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED || checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[] {Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.CAMERA}, 1);
            }
        }
        mOpenCvCameraView = (MRCameraBridgeView) findViewById(R.id.seetaface_surface_view);
        mOpenCvCameraView.setCameraIndex(cameindex);
        mOpenCvCameraView.setMaxFrameSize(640,480);
        mOpenCvCameraView.setVisibility(CameraBridgeViewBase.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_1_0, this, mLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }
    @Override
    protected void onPause() {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mOpenCvCameraView.disableView();
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        mRgba = new Mat();

//        Camera.Size size=mOpenCvCameraView.getResolution();
//        Log.d(TAG,"Resolution:"+size.height+" "+size.width);
    }

    @Override
    public void onCameraViewStopped() {
        mRgba.release();
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        mRgba = inputFrame.rgba();
        if(bseetalibinited)
        {
            try{
                int ret = MRSeetaFaceUtil.seetaFaceDetectFaces(mRgba.nativeObj,mRgba.nativeObj);
                Log.i(TAG,"ret: "+ret);
            }
            catch (Throwable e){
                e.printStackTrace();
            }
        }
        return mRgba;
    }
    public void initFile(){
        MRAssetUtil.CopyAssets(this,"mrface","/sdcard/mrface");
    }
}
