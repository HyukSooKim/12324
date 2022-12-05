package com.example.android_resapi.ui;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import com.example.android_resapi.R;
import com.example.android_resapi.ui.apicall.GetThingShadow;
import com.example.android_resapi.ui.apicall.UpdateShadow;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import java.util.Timer;
import java.util.TimerTask;

public class DeviceActivity extends AppCompatActivity {
    String urlStr;
    final static String TAG = "AndroidAPITest";
    Timer timer;
    Button startGetBtn;
    Button stopGetBtn;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device);
        Intent intent = getIntent();
        urlStr = intent.getStringExtra("thingShadowURL"); //넘겨받은 URL 정보를 뽑아오기

        startGetBtn = findViewById(R.id.startGetBtn);
        startGetBtn.setEnabled(true);
        startGetBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                timer = new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        new GetThingShadow(DeviceActivity.this, urlStr).execute(); //조회 시작 누를때 Async 수행
                    }
                },
                        0,2000);

                startGetBtn.setEnabled(false);
                stopGetBtn.setEnabled(true);
            }
        });

        stopGetBtn = findViewById(R.id.stopGetBtn);
        stopGetBtn.setEnabled(false);
        stopGetBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (timer != null)
                    timer.cancel();
                clearTextView();
                startGetBtn.setEnabled(true);
                stopGetBtn.setEnabled(false);
            }
        });

        Button updateBtn = findViewById(R.id.updateBtn);
        updateBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EditText edit_leds = findViewById(R.id.edit_leds);
                EditText edit_dc = findViewById(R.id.edit_DC);

                JSONObject payload = new JSONObject();

                try {
                    JSONArray jsonArray = new JSONArray();
                    String leds_input = edit_leds.getText().toString();
                    if (leds_input != null && !leds_input.equals("")) {
                        JSONObject tag2 = new JSONObject();
                        tag2.put("tagName", "LEDS");
                        tag2.put("tagValue", leds_input);

                        jsonArray.put(tag2);
                    }

                    String dc_input = edit_dc.getText().toString();
                    if (dc_input != null && !dc_input.equals("")) {
                        JSONObject tag3 = new JSONObject();
                        tag3.put("tagName", "DC_MOTOR");
                        tag3.put("tagValue", dc_input);

                        jsonArray.put(tag3);
                    }

                    if (jsonArray.length() > 0)
                        payload.put("tags", jsonArray);
                } catch (JSONException e) {
                    Log.e(TAG, "JSONEXception");
                }
                Log.i(TAG,"payload="+payload);
                if (payload.length() >0 )
                    new UpdateShadow(DeviceActivity.this,urlStr).execute(payload);
                else
                    Toast.makeText(DeviceActivity.this,"변경할 상태 정보 입력이 필요합니다", Toast.LENGTH_SHORT).show();
            }
        });


    }

    private void clearTextView() {
        TextView reported_tempTV = findViewById(R.id.reported_temp);
        TextView reported_ledsTV = findViewById(R.id.reported_leds);
        TextView reported_DCTV = findViewById(R.id.reported_DC);
        reported_tempTV.setText("");
        reported_DCTV.setText("");
        reported_ledsTV.setText("");

        TextView desired_tempTV = findViewById(R.id.desired_temp);
        TextView desired_ledsTV = findViewById(R.id.desired_leds);
        TextView desired_DCTV = findViewById(R.id.desired_DC);
        desired_tempTV.setText("");
        desired_DCTV.setText("");
        desired_ledsTV.setText("");
    }

}


