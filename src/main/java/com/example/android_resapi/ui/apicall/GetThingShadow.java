package com.example.android_resapi.ui.apicall;

import android.app.Activity;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;
import org.json.JSONException;
import org.json.JSONObject;
import org.w3c.dom.Text;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import com.example.android_resapi.R;
import com.example.android_resapi.httpconnection.GetRequest;

public class GetThingShadow extends GetRequest {
    final static String TAG = "AndroidAPITest";
    String urlStr;
    public GetThingShadow(Activity activity, String urlStr) {
        super(activity);
        this.urlStr = urlStr;
    }

    @Override
    protected void onPreExecute() {
        try {
            Log.e(TAG, urlStr);
            url = new URL(urlStr); //url 객체 초기화 (GetreQuest)

        } catch (MalformedURLException e) {
            Toast.makeText(activity,"URL is invalid:"+urlStr, Toast.LENGTH_SHORT).show();
            e.printStackTrace();
            activity.finish();
        }
    }

    @Override
    protected void onPostExecute(String jsonString) {
        if (jsonString == null)
            return;
        Map<String, String> state = getStateFromJSONString(jsonString); //Json의 형식 바꿔서
        TextView reported_tempTV = activity.findViewById(R.id.reported_temp);
        TextView reported_ledsTV = activity.findViewById(R.id.reported_leds);
        TextView reported_DCTV = activity.findViewById(R.id.reported_DC);
        reported_tempTV.setText(state.get("reported_temp"));
        reported_ledsTV.setText(state.get("reported_LEDS"));
        reported_DCTV.setText(state.get("reported_DC_MOTOR"));

        TextView desired_tempTV = activity.findViewById(R.id.desired_temp);
        TextView desired_ledsTV = activity.findViewById(R.id.desired_leds);
        TextView desired_DCTV = activity.findViewById(R.id.desired_DC);
        desired_tempTV.setText(state.get("desired_temp"));
        desired_DCTV.setText(state.get("desired_DC_MOTOR"));
        desired_ledsTV.setText(state.get("desired_LEDS"));

    }

    protected Map<String, String> getStateFromJSONString(String jsonString) { //서버로받은 Json스트링을 바꿔줌
        Map<String, String> output = new HashMap<>();
        try {
            // 처음 double-quote와 마지막 double-quote 제거
            jsonString = jsonString.substring(1,jsonString.length()-1);
            // \\\" 를 \"로 치환
            jsonString = jsonString.replace("\\\"","\"");
            Log.i(TAG, "jsonString="+jsonString);
            JSONObject root = new JSONObject(jsonString);
            JSONObject state = root.getJSONObject("state");

            JSONObject reported = state.getJSONObject("reported");
            String tempValue = reported.getString("temperature");
            String ledsValue = reported.getString("LEDS");
            String DCValue = reported.getString("DC_MOTOR");
            output.put("reported_temp",tempValue);
            output.put("reported_LEDS", ledsValue);
            output.put("reported_DC_MOTOR",DCValue);

            JSONObject desired = state.getJSONObject("desired");
            String desired_tempValue = reported.getString("temperature");
            String desired_ledsValue = desired.getString("LEDS");
            String desired_DCValue = desired.getString("DC_MOTOR");
            output.put("desired_temp",desired_tempValue);
            output.put("desired_LEDS", desired_ledsValue);
            output.put("desired_DC_MOTOR",desired_DCValue);

        } catch (JSONException e) {
            Log.e(TAG, "Exception in processing JSONString.", e);
            e.printStackTrace();
        }
        return output;
    }
}
