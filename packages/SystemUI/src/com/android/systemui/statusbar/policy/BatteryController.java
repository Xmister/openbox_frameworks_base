/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.systemui.statusbar.policy;

import java.util.ArrayList;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.util.Slog;
import android.widget.ImageView;
import android.widget.TextView;

import com.android.systemui.R;

import android.view.View;

public class BatteryController extends BroadcastReceiver {
    private static final String TAG = "StatusBar.BatteryController";

    private Context mContext;
    private ArrayList<ImageView> mIconViews = new ArrayList<ImageView>();
    private ArrayList<TextView> mLabelViews = new ArrayList<TextView>();

    public BatteryController(Context context) {
        mContext = context;

        IntentFilter filter = new IntentFilter();
        /* delete by Gary. start {{----------------------------------- */
        /* 2012-4-5 */
        /* hide the battery icon in the statusbar */
//        filter.addAction(Intent.ACTION_BATTERY_CHANGED);
        /* delete by Gary. end   -----------------------------------}} */
        context.registerReceiver(this, filter);
    }

    public void addIconView(ImageView v) {
        /* add by Gary. start {{----------------------------------- */
        /* 2012-4-5 */
        /* hide the battery icon in the statusbar */
        v.setVisibility(View.GONE);
        /* add by Gary. end   -----------------------------------}} */
        mIconViews.add(v);
    }

    public void addLabelView(TextView v) {
        /* add by Gary. start {{----------------------------------- */
        /* 2012-4-5 */
        /* hide the battery icon in the statusbar */
        v.setVisibility(View.GONE);
        /* add by Gary. end   -----------------------------------}} */
        mLabelViews.add(v);
    }

    public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();
        if (action.equals(Intent.ACTION_BATTERY_CHANGED)) {
            final int level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
            final boolean plugged = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, 0) != 0;
            final int icon = plugged ? R.drawable.stat_sys_battery_charge 
                                     : R.drawable.stat_sys_battery;
            int N = mIconViews.size();
            for (int i=0; i<N; i++) {
                ImageView v = mIconViews.get(i);
                v.setImageResource(icon);
                v.setImageLevel(level);
                v.setContentDescription(mContext.getString(R.string.accessibility_battery_level,
                        level));
            }
            N = mLabelViews.size();
            for (int i=0; i<N; i++) {
                TextView v = mLabelViews.get(i);
                v.setText(mContext.getString(R.string.status_bar_settings_battery_meter_format,
                        level));
            }
        }
    }
}
