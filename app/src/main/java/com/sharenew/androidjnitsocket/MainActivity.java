package com.sharenew.androidjnitsocket;

import android.graphics.Color;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native_socket");
    }
    Button send;
    Button close;
    Button init;
    EditText ip;
    EditText port;
    EditText sendEdit;
    TextView readText;
    Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what){
                case 11:
                    readText.setText(msg.getData().getString("data"));
                    break;
            }

        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);



        send = (Button) findViewById(R.id.send_button);
        close = (Button) findViewById(R.id.close_button);
        init = (Button) findViewById(R.id.init_button);
        ip = (EditText)findViewById(R.id.ip);
        port = (EditText)findViewById(R.id.port);
        sendEdit = (EditText) findViewById(R.id.send_edit);
        readText = (TextView) findViewById(R.id.read_text);
        init.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                initSocket(ip.getText().toString(),Integer.valueOf(port.getText().toString()));
                init.setText("connected");
                init.setClickable(false);
                init.setBackgroundColor(Color.DKGRAY);
            }
        });
        send.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        sendData(sendEdit.getText().toString().getBytes());
                    }
                }).start();
            }
        });

        close.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        closeSocket();
                    }
                }).start();
            }
        });
        initSocket2();

    }
    public void setRecevieData(byte[] bb){
        Log.d("native_socket","setRecevieData");
        char[] cc = new char[bb.length];
        for(int i=0;i<bb.length;i++){
            cc[i]=(char)bb[i];
        }
        Log.d("native_socket",String.valueOf(cc));
        Message message = new Message();
        message.what = 11;
        Bundle bundle = new Bundle();
        bundle.putString("data",String.valueOf(cc));
        message.setData(bundle);
        handler.sendMessage(message);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        closeSocket();
        closeSocket2();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void initSocket(String ip,int port);
    public native void initSocket2();
    public native void closeSocket();
    public native void closeSocket2();
    public native void sendData(byte[] buffer);
}
