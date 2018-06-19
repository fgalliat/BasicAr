//package hardware.bt;

import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.Writer;
import java.util.Vector;

import javax.bluetooth.DeviceClass;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.DiscoveryListener;
import javax.bluetooth.LocalDevice;
import javax.bluetooth.RemoteDevice;
import javax.bluetooth.ServiceRecord;

/**
 * A simple SPP client that connects with an SPP server
 */

// HC-05 #2 '201603223535'

// works a bit : make EV3 "BT <>" logo

public class BTMACFinder implements DiscoveryListener {

 // object used for waiting
 private static Object lock = new Object();

 // vector containing the devices discovered
 private static Vector vecDevices = new Vector();

 private static String connectionURL = null;

 // ===========================================
 protected static String getDeviceName(RemoteDevice dev) {
  String name;
  try {
   name = dev.getFriendlyName(false);
  } catch (Exception e) {
   name = dev.getBluetoothAddress();
  }
  return name;
 }

 // ===========================================

 public static void main(String[] args) throws IOException {

  BTMACFinder client = new BTMACFinder();

  // display local device address and name
  LocalDevice localDevice = LocalDevice.getLocalDevice();
  System.out.println("Address: " + localDevice.getBluetoothAddress());
  System.out.println("Name: " + localDevice.getFriendlyName());

  // find devices
  DiscoveryAgent agent = localDevice.getDiscoveryAgent();

  System.out.println("Starting device inquiry...");
  agent.startInquiry(DiscoveryAgent.GIAC, client);

  try {
   synchronized (lock) {
    lock.wait();
   }
  } catch (InterruptedException e) {
   e.printStackTrace();
  }

  System.out.println("Device Inquiry Completed. ");

  // print all devices in vecDevices
  int deviceCount = vecDevices.size();

  if (deviceCount <= 0) {
   System.out.println("No Devices Found .");
   //System.exit(0);
  } else {
   // print bluetooth device addresses and names in the format [ No. address
   // (name) ]
   System.out.println("Bluetooth Devices: ");

   PrintWriter etcFile = null;
   // TODO use OpenSystem.fs
   try {
    etcFile = new PrintWriter("/vm_mnt/etc/btDevices.txt");
   } catch (Exception ex) {
   }

   for (int i = 0; i < deviceCount; i++) {
    RemoteDevice remoteDevice = (RemoteDevice) vecDevices.elementAt(i);
    // System.out.println((i+1)+". "+remoteDevice.getBluetoothAddress()+" ("+remoteDevice.getFriendlyName(true)+")");
    System.out.println((i + 1) + ". " + remoteDevice.getBluetoothAddress() + " (" + getDeviceName(remoteDevice) + ")");

    if (etcFile != null) {
     etcFile.println((i + 1) + ". " + remoteDevice.getBluetoothAddress() + " (" + getDeviceName(remoteDevice) + ")");
    }
   }
   if ( etcFile != null ) {
    etcFile.flush();
    etcFile.close();
   }
  }

 }// main

 // methods of DiscoveryListener
 public void deviceDiscovered(RemoteDevice btDevice, DeviceClass cod) {
  // add the device to the vector
  if (!vecDevices.contains(btDevice)) {
   vecDevices.addElement(btDevice);
  }
 }

 // implement this method since services are not being discovered
 public void servicesDiscovered(int transID, ServiceRecord[] servRecord) {
  if (servRecord != null && servRecord.length > 0) {
   connectionURL = servRecord[0].getConnectionURL(0, false);
  }
  synchronized (lock) {
   lock.notify();
  }
 }

 // implement this method since services are not being discovered
 public void serviceSearchCompleted(int transID, int respCode) {
  synchronized (lock) {
   lock.notify();
  }
 }

 public void inquiryCompleted(int discType) {
  synchronized (lock) {
   lock.notify();
  }

 }// end method

}
