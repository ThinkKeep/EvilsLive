package com.thinkkeep.evils;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.NavigationView;
import android.support.design.widget.Snackbar;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.TextUtils;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.thinkkeep.evils.di.component.ActivityComponent;
import com.thinkkeep.evils.di.component.DaggerActivityComponent;
import com.thinkkeep.evils.di.modules.ActivityModule;
import com.thinkkeep.videolib.api.EvilsLiveStreamer;
import com.thinkkeep.videolib.api.EvilsLiveStreamerConfig;
import com.thinkkeep.videolib.jni.EvilsLiveJni;
import com.thinkkeep.videolib.model.CameraSupport;

import javax.inject.Inject;

import butterknife.Bind;
import butterknife.ButterKnife;
import butterknife.OnClick;


public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener, SurfaceHolder.Callback {

    private static final int REQUEST_CODE = 1;
    @Inject
    CameraSupport cameraSupport;

    @Bind(R.id.surface_view)
    SurfaceView mPreviewView;

    private EvilsLiveStreamer streamer;

    private static final String TAG = "MainActivity";

    private int cameraId = 0;

    // 所需的全部权限
    static final String[] PERMISSIONS = new String[]{
            Manifest.permission.CAMERA,
    };

    private PermissionsChecker mPermissionsChecker;

    @Bind(R.id.toolbar) Toolbar mToolbar;
    @Bind(R.id.fab) FloatingActionButton mFab;
    @Bind(R.id.switch_camera) Button mSwitchBtn;
    @Bind(R.id.start) Button startBtn;
    @Bind(R.id.edit_str) EditText urlEdit;

    private boolean isStartStream = true;
    private EvilsLiveStreamerConfig.Builder builder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);

        setSupportActionBar(mToolbar);

        mPermissionsChecker = new PermissionsChecker(this);

        streamer = new EvilsLiveStreamer(this);

        SurfaceHolder holder = mPreviewView.getHolder();
        holder.addCallback(this);

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, mToolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.setDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);
        ActivityComponent activityComponent = DaggerActivityComponent.builder()
                .activityModule(new ActivityModule(this)).build();
        activityComponent.getActivity();

//        CameraComponent cameraComponent = DaggerCameraComponent.builder().activityComponent(activityComponent).
//                cameraModule(new com.thinkkeep.videolib.di.modules.CameraModule(this)).build();
//        cameraComponent.inject(this);
    }

    @OnClick(R.id.switch_camera)
    void onClickSwitchBtn() {
        cameraId = 1 - cameraId;
        start(cameraId);
    }

    @OnClick(R.id.start)
    void onClickStartBtn(){
        if (isStartStream) {
            String url = urlEdit.getText().toString();
            if (TextUtils.isEmpty(url)) {
                url = urlEdit.getHint().toString();
            }
            builder.setStreamUrl(url);
            EvilsLiveStreamerConfig config = builder.build();
            streamer.setStreamConfig(config);
            streamer.startStream();
            startBtn.setText("关闭推流");
            isStartStream = false;
        } else {
            streamer.stopStream();
            startBtn.setText("推送流");
            isStartStream = true;
        }
    }

    @OnClick(R.id.fab)
    void onClickFab(View view) {
        Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                .setAction("Action", null).show();
    }
    @Override
    protected void onResume() {
        super.onResume();
        if (mPermissionsChecker.lacksPermissions(PERMISSIONS)) {
            startPermissionsActivity();
        }
    }

    private void startPermissionsActivity() {
        PermissionsActivity.startActivityForResult(this, REQUEST_CODE, PERMISSIONS);
    }

    private void start(int cameraId) {
        builder = EvilsLiveStreamerConfig.Builder.newBuilder();
        EvilsLiveStreamerConfig config = builder.build();
        streamer.setDisplayPreview(mPreviewView);
        streamer.setStreamConfig(config);
        streamer.stopPreview();
        streamer.startPreview(cameraId);
    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();

        if (id == R.id.nav_camera) {
            // Handle the camera action
            Snackbar.make(getCurrentFocus(), EvilsLiveJni.testJni(), 10000).show();
        } else if (id == R.id.nav_gallery) {

        } else if (id == R.id.nav_slideshow) {

        } else if (id == R.id.nav_manage) {

        } else if (id == R.id.nav_share) {

        } else if (id == R.id.nav_send) {

        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        start(cameraId);
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        surfaceHolder.setFixedSize(i, i1);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        if (streamer != null) {
            streamer.stopPreview();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        // 拒绝时, 关闭页面, 缺少主要权限, 无法运行
        if (requestCode == REQUEST_CODE && resultCode == PermissionsActivity.PERMISSIONS_DENIED) {
            finish();
        }
    }
}
