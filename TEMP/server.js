var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/dotsaout";

MongoClient.connect(url, function(err, db) {
  if (err) throw err;
  console.log("Database created!");
  db.close();
});

MongoClient.connect(url, function(err, db) {
  if (err) throw err;
  var dbo = db.db("dotsaout");
  dbo.createCollection("year", function(err, res) {
    if (err) throw err;
    console.log("Collection created!");
    db.close();
  });
});

MongoClient.connect(url, function(err, db) {
  if (err) throw err;
  var dbo = db.db("dotsaout");
  var myobj = { start: "500", end: "700", energy: "2", gravity: "5", speed: "3", accel: "2", sound: "sound/1.mp3", light: "true", name: "sVosta" };
  dbo.collection("year").insertOne(myobj, function(err, res) {
    if (err) throw err;
    console.log("1 document inserted");
    db.close();
  });
});

const express = require("express");
const app = express();

app.listen(3000, () => {
  console.log("Application started and Listening on port 3000");
});

// serve your css as static
app.use(express.static(__dirname));

app.get("/", (req, res) => {
  res.sendFile(__dirname + "/index.html");
});
