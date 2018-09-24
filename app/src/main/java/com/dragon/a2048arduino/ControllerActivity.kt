package com.dragon.a2048arduino

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.support.v7.app.AppCompatActivity
import android.util.Log
import android.view.View
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_controller.*
import java.io.IOException
import java.util.*

/**
 * Activity for communication with the paired arduino.
 */
class ControllerActivity : AppCompatActivity() {

    var address = ""
    var adapter: BluetoothAdapter? = null
    var socket: BluetoothSocket? = null

    // Bluetooth socket address
    val myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    // Communication with the ui thread.
    val handler = Handler()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_controller)

        // Gets the MAC address of the arduino device sent from the previous activity
        address = intent.getStringExtra("ADDRESS")

        adapter = BluetoothAdapter.getDefaultAdapter()


        // Sets up a thread that listends to input from the arduino
        Thread {
            val device = adapter?.getRemoteDevice(address)
            socket = device?.createInsecureRfcommSocketToServiceRecord(myUUID)
            adapter?.cancelDiscovery()
            try {
                socket?.connect()

            handler.post {

                Toast.makeText(this@ControllerActivity, "Connected", Toast.LENGTH_SHORT).show()
                Thread {
                    try{
                        var inbytes = ByteArray(1024) {0}
                        val input = socket?.inputStream
                        var text = ""
                        while(true) {
                            //val num = input?.read(inbytes)
                            val bit: Char? = input?.read()?.toChar()

                            if(bit != null) {
                                Log.i("INPUT", "$text $bit")
                                if (bit == '\n')
                                    handler.post {
                                        if (!text.contentEquals("Game Over"))
                                            messageView.text = "Score: ${text}"
                                        else
                                            messageView.text = "Game Over.\n Press any button"

                                        text = ""
                                    }
                                else {
                                    text += bit
                                }
                            }
                        }
                    }
                    catch (e: IOException) {

                    }
                    finally {
                        handler.post {
                            Toast.makeText(this@ControllerActivity, "IO done", Toast.LENGTH_SHORT).show()
                        }
                    }
                }.start()
            }
            }
            catch (e: Exception) {
                handler.post {
                    Toast.makeText(this@ControllerActivity, "Failed to connect", Toast.LENGTH_SHORT).show()
                    startActivity(Intent(this as Context, MainActivity::class.java))
                    finish()
                }
            }
        }.start()
    }

    /**
     * User input mapping finctions
     */
    fun up(v: View) {
        socket?.outputStream?.write('8'.toInt())
    }

    fun down(v: View) {
        socket?.outputStream?.write('2'.toInt())
    }

    fun left(v: View) {
        socket?.outputStream?.write('4'.toInt())
    }

    fun right(v: View) {
        socket?.outputStream?.write('6'.toInt())
    }

    fun restart(v: View) {
        socket?.outputStream?.write('r'.toInt())
    }
}
