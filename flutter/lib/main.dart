import 'package:flutter/material.dart';
import 'dart:convert';
import 'package:fluttertoast/fluttertoast.dart';

import 'package:socket_io_client/socket_io_client.dart';
import 'package:socket_io_client/socket_io_client.dart' as io;



void ShowToask(String message)
{
  Fluttertoast.showToast(
      msg: message,
      toastLength: Toast.LENGTH_SHORT,
      gravity: ToastGravity.CENTER,
      timeInSecForIosWeb: 1,
      backgroundColor: Colors.red,
      textColor: Colors.white,
      fontSize: 16.0
  );
}
void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget
{
  const MyApp({super.key});

  @override
  Widget build(BuildContext context)
  {
    return const MaterialApp(
      debugShowCheckedModeBanner: false,
      home: HomeApp(),
    );
  }
}

class HomeApp extends StatefulWidget
{
  const HomeApp({super.key});

  @override
  AppState createState()=> AppState();
}

class AppState extends State<HomeApp>
{
  // khai báo biến

  String nhietdo = "0";
  String doam = "0";

  String imgden = "assets/off.jpg";
  String imgquat = "assets/off.jpg";
  String imgbom = "assets/off.jpg";

  String nodejs_server = "http://192.168.1.27:3000";
  late io.Socket socket;

  String topicsOn = "app"; // nhan du lieu
  String topicpEmit = "flutter";

  String DataSendSocket = "";
  String DataReceived = "";




  // CHẠY 1 LẦN DUY NHẤT hay dùng để kết nối server hoặc khởi tạo hàm timer để lấy dữ liệu
  @override
  void initState()
  {
    super.initState();
    ShowToask("WELCOME TO APP");
    ConnectNodeJs();

  }
  @override
  Widget build(BuildContext context)
  {
    // TẠO GIAO DIỆN trong Scaffold
    return Scaffold(

      appBar: AppBar(
        title: const Row(
          children: <Widget>[
            Image(image:AssetImage('assets/11.jpg',),),
            Text(
              "Dashboard",
              style: TextStyle(
                fontSize: 20
              ),
            ),
            SizedBox(width: 8.0,),
            Image(image:AssetImage('assets/12.jpg',),),
          ],
        ),
      ),

      body: Container(
        padding: const EdgeInsets.fromLTRB(30, 0, 30, 0),
        constraints: const BoxConstraints.expand(),
        color: Colors.white,
        child: SingleChildScrollView(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[

                  const SizedBox(height: 10.0,),

                  const Text(
                    "Nhiệt Độ:",
                    style: TextStyle(
                      fontSize: 20,
                      color: Colors.redAccent,
                    ),
                  ),

                  const SizedBox(width: 8.0,),

                  Text(
                    nhietdo,
                    style: const TextStyle(
                      fontSize: 20,
                      color: Colors.redAccent,
                    ),
                  ),

                  const Image(
                    image: AssetImage('assets/8.jpg',),
                    height: 50,
                    width: 50,
                  ),


                ],
              ),

              const SizedBox(height: 10.0,),

              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[

                  const Text(
                    "Độ Ẩm:",
                    style: TextStyle(
                      fontSize: 20,
                      color: Colors.redAccent,
                    ),
                  ),

                  const SizedBox(width: 8.0,),

                  Text(
                    doam,
                    style: const TextStyle(
                      fontSize: 20,
                      color: Colors.redAccent,
                    ),
                  ),

                  const Image(
                    image: AssetImage('assets/9.jpg',),
                    height: 50,
                    width: 50,
                  ),

                ],
              ),
              const SizedBox(height: 10.0,),

              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[

                  const Text(
                    "Đèn:",
                    style: TextStyle(
                      fontSize: 20,
                      color: Colors.redAccent,
                    ),
                  ),

                  const SizedBox(width: 8.0,),




                  Image(
                    image: AssetImage(imgden,),
                    height: 50,
                    width: 50,
                  ),

                  ElevatedButton(
                    onPressed: DK_DEN,
                    child: const Text("Đèn"),
                  ),

                ],
              ),
              const SizedBox(height: 10.0,),

              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[

                  const Text(
                    "Quạt:",
                    style: TextStyle(
                      fontSize: 20,
                      color: Colors.redAccent,
                    ),
                  ),

                  const SizedBox(width: 8.0,),



                  Image(
                    image: AssetImage(imgquat,),
                    height: 50,
                    width: 50,
                  ),

                  ElevatedButton(
                    onPressed: DK_QUAT,
                    child: const Text("Quạt"),
                  ),
                ],
              ),
              const SizedBox(height: 10.0,),

              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[

                  const Text(
                    "Bơm:",
                    style: TextStyle(
                      fontSize: 20,
                      color: Colors.redAccent,
                    ),
                  ),

                  const SizedBox(width: 8.0,),



                  Image(
                    image: AssetImage(imgbom,),
                    height: 50,
                    width: 50,
                  ),

                  ElevatedButton(
                    onPressed: DK_Bom,
                    child: const Text("Bơm"),
                  ),
                ],
              ),
            ],
          ),
        ),

      ),

    );
  }

  void ConnectNodeJs()
  {
    socket = io.io(nodejs_server, <String, dynamic>{
      'transports': ['websocket'],
      'autoConnect': false,
      'extraHeaders': {'foo': 'bar'} // optional
    });

    // kết nối server nodejs
    socket.connect();

    socket.onConnect((_) {
      print("Connected to Node.js server");
    });

    // lắng nghe sự kiện server gửi về
    socket.on(topicsOn, (data)
    {
      print(data);
      DataReceived = data.toString();

      setState(() {
        var data = DataReceived;

        var DataJsonObject = json.decode(data);

        nhietdo = DataJsonObject['ND'];
        doam = DataJsonObject['DA'];

        if(DataJsonObject['Den'] == '0')
        {

          imgden = "assets/off.jpg";
        }
        else if(DataJsonObject['Den'] == '1')
        {

          imgden = "assets/on.jpg";
        }

        if(DataJsonObject['Quat'] == '0')
        {

          imgquat = "assets/off.jpg";
        }
        else if(DataJsonObject['Quat'] == '1')
        {

          imgquat  = "assets/on.jpg";
        }

        if(DataJsonObject['Bom'] == '0')
        {

          imgbom = "assets/off.jpg";
        }
        else if(DataJsonObject['Bom'] == '1')
        {

          imgbom  = "assets/on.jpg";
        }

      });
    });
  }

  void DK_DEN()
  {
    print("Button Đèn Onclick!!!");

    if(imgden.toString() == "assets/off.jpg")
    {

      socket.emit(topicpEmit, {"Den":"1"});

    }
    else if(imgden.toString() == "assets/on.jpg")
    {

      socket.emit(topicpEmit, {"Den":"0"});

    }
  }

  void DK_Bom() {
    print("Button Bơm Onclick!!!");

    if (imgbom.toString() == "assets/off.jpg") {
      socket.emit(topicpEmit, {"Bom": "1"});
    }
    else if (imgbom.toString() == "assets/on.jpg") {
      socket.emit(topicpEmit, {"Bom": "0"});
    }
  }

  void DK_QUAT() {
    print("Button Quạt Onclick!!!");

    if (imgquat.toString() == "assets/off.jpg") {
      socket.emit(topicpEmit, {"Quat": "1"});
    }
    else if (imgquat.toString() == "assets/on.jpg") {
      socket.emit(topicpEmit, {"Quat": "0"});
    }
  }

}