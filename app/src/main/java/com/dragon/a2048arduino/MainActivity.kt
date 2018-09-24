package com.dragon.a2048arduino

import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.provider.Settings
import android.support.v7.app.AppCompatActivity
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*


class MainActivity : AppCompatActivity(), BluetoothDeviceFragment.OnListFragmentInteractionListener {
    override fun onListFragmentInteraction(item: BluetoothDevice?) {
        if(item != null) {
            val i = Intent(this@MainActivity as Context, ControllerActivity::class.java)
            i.putExtra("ADDRESS", item.address)
            startActivity(i)
            finish()
        }
    }

    lateinit var mBluetoothAdapter: BluetoothAdapter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        main_button_find.setOnClickListener {
            val intentOpenBluetoothSettings = Intent()
            intentOpenBluetoothSettings.action = android.provider.Settings.ACTION_BLUETOOTH_SETTINGS
            startActivity(intentOpenBluetoothSettings)
        }

    }

    override fun onResume() {
        super.onResume()
        if (!mBluetoothAdapter.isEnabled) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT)
        }
        //else
            //doStuff()
    }

    fun doStuff() {
        val pairedDevices = mBluetoothAdapter.bondedDevices
        for(d in pairedDevices) {
            Log.i("Device", d.name)
        }

        val d = pairedDevices.firstOrNull {
            it.name == "HC-05"
        }

        if(d != null) {
            val i = Intent(this@MainActivity as Context, ControllerActivity::class.java)
            i.putExtra("ADDRESS", d.address)
            startActivity(i)
            finish()
        }
//        with(pairedDevices.iterator()) {
//            if(this.hasNext()) {
//                val d = this.next()
//                val i = Intent(this@MainActivity as Context, ControllerActivity::class.java)
//                i.putExtra("ADDRESS", d.address)
//                startActivity(i)
//            }
//            else {
//
//            }
//        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if(requestCode == REQUEST_ENABLE_BT) {
            if(resultCode == Activity.RESULT_OK) {
                recreate()
            }
        }
    }

    companion object {
        val REQUEST_ENABLE_BT = 1
    }
}
