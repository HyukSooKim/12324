package com.example.android_resapi.httpconnection;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

import javax.net.ssl.HttpsURLConnection;

abstract public class GetRequest extends AsyncTask<String, Void, String> {
    final static String TAG = "AndroidAPITest";
    protected Activity activity;
    protected URL url;

    public GetRequest(Activity activity) {
        this.activity = activity;
    }


    @Override
    protected String doInBackground(String... strings) {
        StringBuffer output = new StringBuffer();

        try {
            if (url == null) {
                Log.e(TAG, "Error: URL is null ");
                return null;
            }
            HttpURLConnection conn = (HttpURLConnection) url.openConnection(); //서버랑 connection

            if (conn == null) {
                Log.e(TAG, "HttpsURLConnection Error");
                return null;
            }
            conn.setConnectTimeout(10000); //필요한 설정 시작
            conn.setRequestMethod("GET");
            conn.setDoInput(true);
            conn.setDoOutput(false); //필요한 설정 끝

            int resCode = conn.getResponseCode(); //서버로부터 응답을 받는 부분

            if (resCode != HttpsURLConnection.HTTP_OK) {
                Log.e(TAG, "HttpsURLConnection ResponseCode: " + resCode);
                conn.disconnect();
                return null;
            }
            BufferedReader reader = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            String line = null;
            while (true) {
                line = reader.readLine(); //한줄씩읽어서
                if (line == null) {
                    break;
                }
                output.append(line); //output 이라 하는 String BUffer에 추가
            }

            reader.close();
            conn.disconnect();

        } catch (IOException ex) {
            Log.e(TAG, "Exception in processing response.", ex);
            ex.printStackTrace();
        }

        return output.toString(); //한번에 저장된 큰 문자열을 결과로 리턴한다 , post Execute의 파라미터로 자동으로 전달
    }

}
