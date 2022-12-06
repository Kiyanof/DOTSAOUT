const tooltipTriggerList = document.querySelectorAll('[data-bs-toggle="tooltip"]')
const tooltipList = [...tooltipTriggerList].map(tooltipTriggerEl => new bootstrap.Tooltip(tooltipTriggerEl))

var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/dotsaout";

MongoClient.connect(url, function(err, db) {
  if (err) throw err;
  var dbo = db.db("dotsaout");
  dbo.collection("year").find({}).toArray(function(err, result) {
    if (err) throw err;
    console.log(result);
    db.close();
  });
});

const net = "http://192.168.1."

var energyIP = net + "26"
var gravityIP = net + "27"
var speedIP = net + "28"
var accelIP = net + "29"

var yearIP = net + "23"
var timerIP = net + "24"

var stageIP = [net+"20", net+"21", net+"22"]

var swIP = net + "30"
var gIP = net + "31"
var rtIP = net + "33"
var swtIP = net + "32"
var wIP = net + "34"
var gjIP = net + "38"

var feleroIP = net + "37"

var robotIP = net + "25"

var greens = ['A3', 'B2','C4','D3','E5','F5','G4','H2']
function checkGreens(inp) {
  for (var cell in greens) {
    if(greens[cell] == inp) return true
  }
  return false;
}

//if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
//else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }

let stageLink = [
  [stageIP[0]+"/update?relay="+1,stageIP[0]+"/update?relay="+9, stageIP[0]+"/update?relay="+11, stageIP[0]+"/update?relay="+16, stageIP[1]+"/update?relay="+5, stageIP[1]+"/update?relay="+10, stageIP[1]+"/update?relay="+15, stageIP[2]+"/update?relay="+4],
  [stageIP[0]+"/update?relay="+2,stageIP[2]+"/update?relay="+10, stageIP[0]+"/update?relay="+12, stageIP[1]+"/update?relay="+1, stageIP[1]+"/update?relay="+6, stageIP[1]+"/update?relay="+11, stageIP[1]+"/update?relay="+16, stageIP[2]+"/update?relay="+16],
  [stageIP[2]+"/update?relay="+9,stageIP[0]+"/update?relay="+7, stageIP[0]+"/update?relay="+13, stageIP[2]+"/update?relay="+12, stageIP[1]+"/update?relay="+7, stageIP[1]+"/update?relay="+12, stageIP[2]+"/update?relay="+1, stageIP[2]+"/update?relay="+5],
  [stageIP[0]+"/update?relay="+3,stageIP[0]+"/update?relay="+8, stageIP[2]+"/update?relay="+11, stageIP[1]+"/update?relay="+2, stageIP[1]+"/update?relay="+8, stageIP[1]+"/update?relay="+13, stageIP[2]+"/update?relay="+15, stageIP[2]+"/update?relay="+6],
  [stageIP[0]+"/update?relay="+4,stageIP[0]+"/update?relay="+6, stageIP[0]+"/update?relay="+14, stageIP[1]+"/update?relay="+3, stageIP[2]+"/update?relay="+13, stageIP[2]+"/update?relay="+14, stageIP[2]+"/update?relay="+2, stageIP[2]+"/update?relay="+7],
  [stageIP[0]+"/update?relay="+5,stageIP[0]+"/update?relay="+10, stageIP[0]+"/update?relay="+15, stageIP[1]+"/update?relay="+4, stageIP[1]+"/update?relay="+9, stageIP[1]+"/update?relay="+14, stageIP[2]+"/update?relay="+3, stageIP[2]+"/update?relay="+8]
]

function torch(obj) {
  var state = $(obj).attr("state")
  var color = $(obj).attr("color")
  var row = $(obj).attr("row")
  var col = $(obj).attr("col")

  var xhr = new XMLHttpRequest();

  if (state == "off") {
    $(obj).attr("state", "on")
    xhr.open("GET", stageLink[row][col]+"&state=1", true);
    if (color == "green") {
      $(obj).addClass("stage-color-green")
    }
    else if (color == "red"){
        $(obj).addClass("stage-color-red")
    }
  }
  else {
    $(obj).attr("state", "off")
      xhr.open("GET", stageLink[row][col]+"&state=0", true);
    if (color == "green") {
      $(obj).removeClass("stage-color-green")
    }
    else if (color == "red") {
        $(obj).removeClass("stage-color-red")
    }
  }

  xhr.send();
}

function createBombTable() {
  var row = 0
  var col = 0
  var maxRow = 6
  var maxCol = 8
  var y = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
  for (var i = 0; i <6; i++) {
    col = 0
    $('#table_bomb').append('<tr class="table-dark" id="row' + (i+1) + '"></tr>')
    for (var j = 0; j < maxCol; j++) {
      if (checkGreens(y[j] + (i + 1))) {
        $('#row' + (i+1)).append('<td color="green" row="' + row + '" col="' + col + '" class="text-center green h-80 border border-gold color amber" state="off" onclick="torch(this)" id="col' + (j+1) + '">' + y[j] + (i+1) +'</td>')
      } else {
        $('#row' + (i+1)).append('<td color="red" row="' + row + '" col="' + col + '" class="text-center h-80 border border-gold color amber" state="off" onclick="torch(this)" id="col' + (j+1) + '">' + y[j] + (i+1) +'</td>')
      }
      col++;
    }
    row++;
  }
}

function win() {
    var xhr = new XMLHttpRequest();
    for (var i = 9; i < 16; i++) {
      xhr.open("GET", stageIP[2]+"/update?relay="+i+"state=1", true);
      xhr.send()
    }
    $("#winBtn").addClass("btn-outline-success")
    $("#winBtn").removeClass("btn-success")

    setTimeout(function() {
      for (var i = 9; i < 16; i++) {
        xhr.open("GET", stageIP[2]+"/update?relay="+i+"state=0", true);
        xhr.send()
      }
      $("#winBtn").addClass("btn-success")
      $("#winBtn").removeClass("btn-outline-success")
    }, 50);

}

function getRndInteger(min, max) {
  return Math.floor(Math.random() * (max - min) ) + min;
}

function loose() {
  var xhr = new XMLHttpRequest();
  var randRelay = getRndInteger(0, 15)
  var randStage = getRndInteger(0, 2)
  while (randRelay >= 9 && randStage == 2) {
    randRelay = getRndInteger(0, 15)
  }
  xhr.open("GET", stageIP[randStage]+"/update?relay="+randRelay+"state=1", true);
  xhr.send()

  $("#looseBtn").addClass("btn-outline-danger")
  $("#looseBtn").removeClass("btn-danger")

  setTimeout(function() {
    xhr.open("GET", stageIP[randStage]+"/update?relay="+randRelay+"state=0", true);
    xhr.send()
    $("#looseBtn").addClass("btn-danger")
    $("#looseBtn").removeClass("btn-outline-danger")

  }, 50);

}

var winInterval;
var looseInterval;

function reset() {
  var xhr = new XMLHttpRequest();
  clearInterval(winInterval);
  clearInterval(looseInterval);
  for (var i = 0; i < 6; i++) {
    for (var j = 0; j < 8; j++) {
      xhr.open("GET", stageLink[i][j]+"&state=0", true);
      xhr.send()
    }
  }
  $("#looseBtn").addClass("btn-outline-danger")
  $("#looseBtn").removeClass("btn-danger")
  $("#winBtn").addClass("btn-outline-success")
  $("#winBtn").removeClass("btn-success")

}

createBombTable();

function bulbTorch(obj) {
  var state = $(obj).attr("state");
  if (state == 'off') {
    $(obj).attr("src", "icons/lightbulb.svg");
    $(obj).attr("state", "on")
  }
  else {
    $(obj).attr("src", "icons/lightbulb-off.svg");
    $(obj).attr("state", "off")
  }

}

function toggleDoor(obj) {
  var state = $(obj).attr("state");
  if (state == 'closed') {
    $(obj).attr("src", "icons/door-open.svg");
    $(obj).attr("state", "open")
  }
  else {
    $(obj).attr("src", "icons/door-closed.svg");
    $(obj).attr("state", "closed")
  }

}

function btnActive(i, obj, state) {
  if (state) {
    rc = "btn-outline-"
    ac = "btn-"
  } else {
    rc = "btn-"
    ac = "btn-outline-"
  }
  if (i <= 3) {
    $(obj).removeClass(rc + "danger")
    $(obj).addClass(ac + "danger")
  } else if(i <= 6) {
    $(obj).removeClass(rc + "warning")
    $(obj).addClass(ac + "warning")
  } else if (i <= 10) {
    $(obj).removeClass(rc + "success")
    $(obj).addClass(ac + "success")
  } else {
    console.log("ERROR in btnToggle()")
  }
}

function mixerLoad(name, volume) {
    $("#"+name+"_lbl").text(volume);
    $("#"+name+"Btns").children().each(function(index) {
      if (10 - index <= volume) {
        btnActive(index, this, true)
      } else {
        btnActive(index, this, false)
      }
    });
}

$("#feleroTorchSpot").children("span").click(function () {
  var state = $(this).attr("state")
  var name  = $(this).attr("name")
  var xhr = new XMLHttpRequest();
  var id = name.charAt(name.length-1)
  if (state == 'off') {
    xhr.open("GET", feleroIP + "/update?relay="+id+"&state="+"1", true);
    $(this).attr("state", "on")
    $(this).toggleClass("bg-color lime")
  }
  else {
    xhr.open("GET", feleroIP + "/update?relay="+id+"&state="+"0", true);
    $(this).attr("state", "off")
    $(this).toggleClass("bg-color lime")
  }
  xhr.send();
})

var faceMode = 0
var faceInterval

$("#robotFace").children().children().click(function () {
  var xhr = new XMLHttpRequest();
  var state = $(this).attr("state")
  var prevSrc = $(this).attr("src")
  var temp = prevSrc.split(".")
  var newSrc = temp[0] + "-fill." + temp[1]
  if (state == "off") {
    $(this).attr("src", newSrc)
    $(this).attr("state", "on")

  }

  if (faceMode != 0) {
    var prevMode = $('[mode=' + faceMode + ']')
    if (prevMode.attr("mode") == 8) {
      clearInterval(faceInterval)
      prevMode.removeClass("btn-light")
      prevMode.addClass("btn-outline-light")
    }
    else {
      var src = prevMode.attr("src").replace('-fill', '')
      prevMode.attr("src", src)
    }
    prevMode.attr("state", "off")
  }

  faceMode = + $(this).attr("mode")
  xhr.open("GET", robotIP + "/update?face="+faceMode-1, true);
  xhr.send()
})

function robotMove(obj) {
  var xhr = new XMLHttpRequest();
  var prevMode = $('[mode=' + faceMode + ']')
  var src = prevMode.attr("src").replace('-fill', '')
  prevMode.attr("src", src)
  prevMode.attr("state", "off")

  var state = $(obj).attr("state")
  faceMode = + $(obj).attr("mode")

  $(obj).removeClass("btn-outline-light")
  $(obj).addClass("btn-light")

  faceInterval = setInterval(
    function () {
      xhr.open("GET", robotIP + "/update?face="+faceMode-1, true);
      xhr.send()
    }, getRndInteger(70, 300)
  )
}






var energy = new EventSource(energyIP + '/events');
var gravity = new EventSource(gravityIP + '/events');
var speed = new EventSource(speedIP + '/events');
var accel = new EventSource(accelIP + '/events');
var year = new EventSource(yearIP + '/events');
var timer = new EventSource(timerIP + '/events');
var sw = new EventSource(swIP + '/events');
var g = new EventSource(gIP + '/events');
var rt = new EventSource(rtIP + '/events');
var swt = new EventSource(swtIP + '/events');
var w = new EventSource(wIP + '/events');
var gj = new EventSource(gjIP + '/events');

sw.addEventListener('skill', function(e) {
  $("#stopwatch_max_lbl").text(e.data)
}, false);
sw.addEventListener('timer', function(e) {
  $("#stopwatch_time_lbl").text(e.data)
}, false);
g.addEventListener('skill', function(e) {
  $("#gaurd_max_lbl").text(e.data)
}, false);
g.addEventListener('timer', function(e) {
  $("#gaurd_time_lbl").text(e.data)
}, false);
rt.addEventListener('skill', function(e) {
  $("#tek_max_lbl").text(e.data)
}, false);
rt.addEventListener('timer', function(e) {
  $("#tek_time_lbl").text(e.data)
}, false);
swt.addEventListener('skill', function(e) {
  $("#switcher_max_lbl").text(e.data)
}, false);
swt.addEventListener('timer', function(e) {
  $("#switcher_time_lbl").text(e.data)
}, false);
w.addEventListener('skill', function(e) {
  $("#wise_max_lbl").text(e.data)
}, false);
w.addEventListener('timer', function(e) {
  $("#wise_time_lbl").text(e.data)
}, false);
gj.addEventListener('skill', function(e) {
  $("#gadjet_max_lbl").text(e.data)
}, false);
gj.addEventListener('timer', function(e) {
  $("#gadjet_time_lbl").text(e.data)
}, false);

energy.addEventListener('volume', function(e) {
  mixerLoad('energy', e.data)
}, false);
gravity.addEventListener('volume', function(e) {
  mixerLoad('gravity', e.data)
}, false);
speed.addEventListener('volume', function(e) {
  mixerLoad('speed', e.data)
}, false);
accel.addEventListener('volume', function(e) {
  mixerLoad('accel', e.data)
}, false);

var lunchInterval
function lunch() {
  $("#lunchBtn").removeClass("btn-outline-primary")
  $("#lunchBtn").addClass("btn-primary")

  setTimeout(function() {
    $("#lunchBtn").removeClass("btn-primary")
    $("#lunchBtn").addClass("btn-outline-primary")
  }, 50);
}
year.addEventListener('mode', function(e) {
  if (e.data == 1) {
    lunchInterval = setInterval(lunch, 300)
  }
  else {
    clearInterval(lunchInterval)
    $("#lunchBtn").removeClass("btn-primary")
    $("#lunchBtn").addClass("btn-outline-primary")
  }

 }, false);
year.addEventListener('num', function(e) {
  $("#year_lbl").text(e.data);
 }, false);
timer.addEventListener('minute', function(e) {
   $("#minute_lbl").text(e.data);
  }, false);
timer.addEventListener('second', function(e) {

    $("#second_lbl").text(e.data);
   }, false);


function heroActive(ip, relay, obj) {
  var state = $(obj).attr("state")
  var xhr = new XMLHttpRequest();
  if (state == "off") {
    xhr.open("GET", ip + "/update?relay="+relay+"&state="+1, true);
    $(obj).attr("state", "on")
    if (relay == 1) {
      $(obj).removeClass("btn-outline-secondary")
      $(obj).addClass("btn-secondary")
    }
    else if (relay == 2) {
      $(obj).removeClass("btn-outline-danger")
      $(obj).addClass("btn-danger")
    }

  } else {
    xhr.open("GET", ip + "/update?relay="+relay+"&state="+0, true);
    $(obj).attr("state", "off")

    if (relay == 1) {
      $(obj).removeClass("btn-secondary")
      $(obj).addClass("btn-outline-secondary")
    }
    else if (relay == 2) {
      $(obj).removeClass("btn-danger")
      $(obj).addClass("btn-outline-danger")
    }
  }
  xhr.send();
}

function skill_conf(ip, name){
  var max = $("#" + name + "_max_inp").val()
  var time = $("#" + name + "_time_inp").val()
  var xhr = new XMLHttpRequest();
  xhr.open("GET", ip + "/update?skill="+max+"&timer="+time, true);
  xhr.send();
  $("#" + name + "_max_lbl").text(max)
  $("#" + name + "_time_lbl").text(time)
}

function timer_conf(minute, second) {
   var xhr = new XMLHttpRequest();
   xhr.open("GET", timerIP + "/update?minute="+minute+"&second="+second, true);
   xhr.send();
   $("#minute_lbl").text(minute);
   $("#second_lbl").text(second);
 }
 function minDec(inp) {
   var newMinute = + $("#minute_lbl").text() - inp
   timer_conf(newMinute,$("#second_lbl").text())
 }
 function minInc(inp) {
   var newMinute = + $("#minute_lbl").text() + inp
   timer_conf(newMinute,$("#second_lbl").text())
 }
function timer_start() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", timerIP + "/update?start=1", true);
  xhr.send();
}
function timer_stop() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", timerIP + "/update?start=0", true);
  xhr.send();
}
