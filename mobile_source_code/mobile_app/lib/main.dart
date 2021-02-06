import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';
import 'dart:math';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'dart:ui';
import 'dart:ui' as ui;


class BouncingBallDemo extends StatefulWidget {
  _BouncingBallDemoState createState() => _BouncingBallDemoState();
}

class _BouncingBallDemoState extends State<BouncingBallDemo> {
  String _ipAddress = 'Unknown';
  double realX;
  double realY;
  double x;
  double y;
  double h;
  double size;
  Container tempC;
  void changePosition(Timer t) async {
    setState(() {
      _getIPAddress();

    });
  }

  void initState() {
    super.initState();
    tempC = new Container(
      child : Container(
        decoration: BoxDecoration(
          image: DecorationImage(
            image: AssetImage("assets/images/grid2.jpg"),
            fit: BoxFit.cover,
          ),
          shape: BoxShape.circle,
          color: Colors.amber,
          border: Border.all(color: Colors.yellow),
        ),),

    );


    new Timer.periodic(const Duration(milliseconds: 100), changePosition);
  }

  _getIPAddress() async {
    String url = 'http://192.168.4.1';

    var response = await http.read(url);
    Map data = json.decode(response);
    List l = data.values.toList();
    realX = l[0].toDouble();
    realY = l[1].toDouble();
    h = l[2].toDouble();
    //x = (3.7)/(2.0) * realX;
   // y = (3.7)/(2.0) * realY;
    size=(h - 0) * (120 - 60) / (40 - 0) + 60;
    x =((realX- 0) * (450 - 0) / (200 - 0) + 0)-(size/2);
    y = ((realY- 0) * (450 - 0) / (200 - 0) + 0)-(size/2);
    String ip = data['origin'];

    // If the widget was removed from the tree while the message was in flight,
    // we want to discard the reply rather than calling setState to update our
    // non-existent appearance.
    if (!mounted) return;

    setState(() {
      _ipAddress = ip;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        backgroundColor: Colors.white,
        appBar: AppBar(
          flexibleSpace: Image(
            image: AssetImage("assets/images/bck.jpg"),
            fit: BoxFit.cover,
          ),
          backgroundColor: Colors.transparent,

          title: Row(
              children: <Widget>[
                Icon(
                  Icons.android,
                  color: Colors.lightGreen,
                  size: 35.0,
                ) ,
                Text(
                  "    PING PONG BOUNCER",
                  style: TextStyle(
                      fontSize: 20,
                      foreground: Paint()
                        ..shader = ui.Gradient.linear(
                          const Offset(0, 20),
                          const Offset(150, 20),
                          <Color>[
                            Colors.red,
                            Colors.yellow,
                          ],
                        )
                  ),
                  textAlign: TextAlign.center,
                ),

              ]
          ),

        ),

        body: Center(

            child: Stack(children: <Widget>[
              Container(
                width: 450,
                height: 450,
                color: Colors.white,
              ),
              Container(
                  width: 450,
                  height: 450,
                  child: new FlutterLogo()
              ),


              ClipRect(
                  child: new BackdropFilter(
                      filter: new ImageFilter.blur(sigmaX: 10.0, sigmaY: 10.0),
                      child: new Container(
                          width: 450,
                          height: 450,
                          decoration: new BoxDecoration(
                              color: Colors.blueAccent.shade200.withOpacity(0.5),
                              border: Border.all(color: Colors.black)
                          )))) ,

              Positioned(
                top: y,
                left: x,
                width: size,
                height: size,
                child: tempC,
              )


            ],)

        ),


        bottomNavigationBar: Container(
          //color: Colors.teal,//orange.withOpacity(0.8) ,
          decoration: BoxDecoration(
            image: DecorationImage(
              image: AssetImage("assets/images/bck.jpg"),
              fit: BoxFit.cover,
            ),
          ),
          child: Text(
            '\nX: $realX\nY: $realY\nH: $h\n',
            textAlign: TextAlign.center,
            textScaleFactor: 2,
            style: TextStyle(
                fontSize: 15,
                foreground: Paint()
                  ..shader = ui.Gradient.linear(
                    const Offset(0, 20),
                    const Offset(150, 20),
                    <Color>[
                      Colors.red,
                      Colors.yellow,
                    ],
                  )
            ),),


        )
    );

  }
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        debugShowCheckedModeBanner: false,
        home: BouncingBallDemo()
    );
  }
}

Future<void> main() async {
  runApp(
    MyApp(),
  );
}


