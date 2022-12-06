if (document.addEventListener) {
  document.addEventListener('contextmenu', function(e) {
    alert("You've tried to open context menu"); //here you draw your own menu
    e.preventDefault();
  }, false);
} else {
  document.attachEvent('oncontextmenu', function() {
    alert("You've tried to open context menu");
    window.event.returnValue = false;
  });
}

var audioElement = document.createElement('audio');
audioElement.setAttribute('src', 'sound/test.mp3');

const tooltipTriggerList = document.querySelectorAll('[data-bs-toggle="tooltip"]')
const tooltipList = [...tooltipTriggerList].map(tooltipTriggerEl => new bootstrap.Tooltip(tooltipTriggerEl))

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

var timeGIP = net + "40"

var years = [529, 1488, 2072, 2090]

var energyVal = [3, 5, 7, 9, 0]
var gravityVal = [8, 4, 6, 2, 0]
var speedVal = [5, 7, 9, 10, 0]
var accelVal = [4, 8, 6, 9, 0]

var notConf = 1;
var year_id = 0;

var feleroTorchs = [15 * 1000, 30 * 1000, 45 * 1000, 60 * 1000, 20 * 1000]

var timeGadjetTimerMin = 1 // min
var timeGadjetTimerSec = 10 // min
var timeGadjetCondition = true
var timeGadjetTimerInterval

function timeGadjetTimerSet(min, sec) {
  timeGadjetTimerMin = + min
  timeGadjetTimerSec = + sec
  $("#tg_minute_lbl").text(min)
  $("#tg_second_lbl").text(sec)
}

function timeGadjetTimer() {
  timeGadjetCondition = false
   timeGadjetTimerInterval = setInterval(function () {
    if (timeGadjetTimerSec >= 0) timeGadjetTimerSec--;
    else {
      if (timeGadjetTimerMin > 0) {
        timeGadjetTimerSec = 59
        timeGadjetTimerMin--
      }
      else {
        timeGadjetCondition = true
        clearInterval(timeGadjetTimerInterval)
      }
    }
    if (timeGadjetTimerSec != -1) {
      $("#tg_minute_lbl").text(timeGadjetTimerMin)
      $("#tg_second_lbl").text(timeGadjetTimerSec)
    }


  }, 1000);
}

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

function createYearTable() {
  for (var i = 0; i < yearObj.length; i++) {
    $('#table_year').append('<tr class="table-dark" id="yrow' + (i+1) + '"></tr>')
    $('#yrow' + (i+1)).append('<td class="text-center green h-80 border border-gold color amber">' + i + '</td>')
    $('#yrow' + (i+1)).append('<td class="text-center green h-80 border border-gold color amber">' + yearObj[i]["name"] + '</td>')
    $('#yrow' + (i+1)).append('<td class="text-center green h-80 border border-gold color amber">' + yearObj[i]["start"] + '</td>')
    $('#yrow' + (i+1)).append('<td class="text-center green h-80 border border-gold color amber">' + yearObj[i]["end"] + '</td>')
    $('#yrow' + (i+1)).append('<td class="text-center green h-80 border border-gold color amber">' + yearObj[i]["sound"] + '</td>')

  }
}

createYearTable()

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
    var xhr1 = new XMLHttpRequest();
    var xhr2 = new XMLHttpRequest();
    var xhr3 = new XMLHttpRequest();
    var xhr4 = new XMLHttpRequest();
    var xhr5 = new XMLHttpRequest();
    var xhr6 = new XMLHttpRequest();
    var xhr7 = new XMLHttpRequest();
    var xhr8 = new XMLHttpRequest();

    setTimeout(function () {
      xhr1.open("GET", stageIP[2]+"/update?relay=9&state=1", true); xhr1.send()
    }, 50)
    setTimeout(function () {
      xhr2.open("GET", stageIP[2]+"/update?relay=10&state=1", true); xhr2.send()
    }, 150)
    setTimeout(function () {
      xhr3.open("GET", stageIP[2]+"/update?relay=11&state=1", true); xhr3.send()
    }, 250)
    setTimeout(function () {
      xhr4.open("GET", stageIP[2]+"/update?relay=12&state=1", true); xhr4.send()
    }, 350)
    setTimeout(function () {
      xhr5.open("GET", stageIP[2]+"/update?relay=13&state=1", true); xhr5.send()
    }, 450)
    setTimeout(function () {
      xhr6.open("GET", stageIP[2]+"/update?relay=14&state=1", true); xhr6.send()
    }, 550)
    setTimeout(function () {
      xhr7.open("GET", stageIP[2]+"/update?relay=15&state=1", true); xhr7.send()
    }, 650)
    setTimeout(function () {
      xhr8.open("GET", stageIP[2]+"/update?relay=16&state=1", true); xhr8.send()
    }, 750)
/*
    xhr2.open("GET", stageIP[2]+"/update?relay=10&state=1", false); xhr2.send()
    xhr3.open("GET", stageIP[2]+"/update?relay=11&state=1", false); xhr3.send()
    xhr4.open("GET", stageIP[2]+"/update?relay=12&state=1", false); xhr4.send()
    xhr5.open("GET", stageIP[2]+"/update?relay=13&state=1", false); xhr5.send()
    xhr6.open("GET", stageIP[2]+"/update?relay=14&state=1", false); xhr6.send()
    xhr7.open("GET", stageIP[2]+"/update?relay=15&state=1", false); xhr7.send()
    xhr8.open("GET", stageIP[2]+"/update?relay=16&state=1", false); xhr8.send()
*/

    $("#winBtn").addClass("btn-outline-success")
    $("#winBtn").removeClass("btn-success")

    setTimeout(function() {
      setTimeout(function () {
        xhr1.open("GET", stageIP[2]+"/update?relay=9&state=0", true); xhr1.send()
      }, 50)
      setTimeout(function () {
        xhr2.open("GET", stageIP[2]+"/update?relay=10&state=0", true); xhr2.send()
      }, 150)
      setTimeout(function () {
        xhr3.open("GET", stageIP[2]+"/update?relay=11&state=0", true); xhr3.send()
      }, 250)
      setTimeout(function () {
        xhr4.open("GET", stageIP[2]+"/update?relay=12&state=0", true); xhr4.send()
      }, 350)
      setTimeout(function () {
        xhr5.open("GET", stageIP[2]+"/update?relay=13&state=0", true); xhr5.send()
      }, 450)
      setTimeout(function () {
        xhr6.open("GET", stageIP[2]+"/update?relay=14&state=0", true); xhr6.send()
      }, 3550)
      setTimeout(function () {
        xhr7.open("GET", stageIP[2]+"/update?relay=15&state=0", true); xhr7.send()
      }, 650)
      setTimeout(function () {
        xhr8.open("GET", stageIP[2]+"/update?relay=16&state=0", true); xhr8.send()
      }, 450)
      $("#winBtn").addClass("btn-success")
      $("#winBtn").removeClass("btn-outline-success")
    }, 1000);

}

function getRndInteger(min, max) {
  return Math.floor(Math.random() * (max - min) ) + min;
}

function loose() {
  // NOT USE NOW
  var xhr = new XMLHttpRequest();
  var randRelay = getRndInteger(0, 15)
  var randStage = getRndInteger(0, 2)
  while (randRelay >= 9 && randStage == 2) {
    randRelay = getRndInteger(0, 15)
  }
  setTimeout(function () {
    xhr.open("GET", stageIP[randStage]+"/update?relay="+randRelay+"&state=1", false);
    xhr.send()
  }, 250)

  $("#looseBtn").addClass("btn-outline-danger")
  $("#looseBtn").removeClass("btn-danger")

  setTimeout(function() {
    xhr.open("GET", stageIP[randStage]+"/update?relay="+randRelay+"&state=0", false);
    xhr.send()
    $("#looseBtn").addClass("btn-danger")
    $("#looseBtn").removeClass("btn-outline-danger")

  }, 300);

}

var winInterval;
var looseInterval;

function reset() {
  var xhr = new XMLHttpRequest();
  clearInterval(winInterval);
  clearInterval(looseInterval);
  for (var i = 0; i < 6; i++) {
    for (var j = 0; j < 8; j++) {
      xhr.open("GET", stageLink[i][j]+"&state=0", false);
      xhr.send()
    }
  }
  //$("#looseBtn").addClass("btn-outline-danger")
  //$("#looseBtn").removeClass("btn-danger")
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
  var obj = this
  var id = name.charAt(name.length-1)
  if (state == 'off') {
    xhr.open("GET", feleroIP + "/update?relay="+id+"&state="+"1", true);
    $(this).attr("state", "on")
    $(this).toggleClass("bg-color lime")
    setTimeout(function(){
      var xhr2 = new XMLHttpRequest();
      xhr2.open("GET", feleroIP + "/update?relay="+id+"&state="+"0", true);
      $(obj).attr("state", "off")
      $(obj).toggleClass("bg-color lime")
      xhr2.send();
    }, feleroTorchs[id-1]);
  }
  else {
    xhr.open("GET", feleroIP + "/update?relay="+id+"&state="+"0", true);
    $(this).attr("state", "off")
    $(this).toggleClass("bg-color lime")
  }
  xhr.send();
})

function feleroTorchTime() {
  for (var i = 0; i < feleroTorchs.length; i++) {
    feleroTorchs[i] = + ($("#T" + (i+1)).val()) * 1000
  }
}

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
  xhr.open("GET", robotIP + "/update?face="+(faceMode-1), true);
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
      xhr.open("GET", robotIP + "/update?face="+(faceMode-1), true);
      xhr.send()
    }, 50)
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
var timeGadjet = new EventSource(timeGIP + '/events')

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

function timeGadjetReset() {
  timeGadjetCondition = true
  clearInterval(timeGadjetTimerInterval)
}

function add_time(i, state) {
  if (timeGadjetCondition) {
    if (state == '0') {
      timeGadjetTimer()
      $("#t" + i).removeClass("btn-outline-secondary")
      $("#t" + i).addClass("btn-secondary")
      if (i >= 1 && i <= 4) {
        var newMin = 3
        minInc(newMin)
      }
      else {
        var newMin = 5
        minInc(newMin)
      }
    }
    else {
      $("#t" + i).addClass("btn-outline-secondary")
      $("#t" + i).removeClass("btn-secondary")

      if (i >= 1 && i <= 4) {
        var newMin = 3
        minDec(newMin)
      }
      else {
        var newMin = 5
        minDec(newMin)
      }
    }
  }
  else {
    minDec(1)
  }

}

timeGadjet.addEventListener('1', function (e) {
  add_time(1, e.data)

}, false);
timeGadjet.addEventListener('2', function (e) {
  add_time(2, e.data)

}, false);
timeGadjet.addEventListener('3', function (e) {
  add_time(3, e.data)

}, false);
timeGadjet.addEventListener('4', function (e) {
  add_time(4, e.data)
}, false);
timeGadjet.addEventListener('5', function (e) {
  add_time(5, e.data)

}, false);
timeGadjet.addEventListener('6', function (e) {
  add_time(6, e.data)

}, false);
timeGadjet.addEventListener('7', function (e) {
  add_time(7, e.data)

}, false);
timeGadjet.addEventListener('8', function (e) {
  add_time(8, e.data)
}, false);

sw.addEventListener('skill', function(e) {
  $("#stopwatch_max_lbl").text(e.data)
}, false);
sw.addEventListener('timer', function(e) {
  $("#stopwatch_time_lbl").text(e.data)
}, false);
sw.addEventListener('active', function(e) {
  if (e.data == 1) {
    $("#swCard").toggleClass("border-color-red")
  } else {
    $("#swCard").toggleClass("border-color-red")
  }
}, false);
sw.addEventListener('corom', function(e) {
  alert("hello")
  if (e.data == 1) {
    var max = + $("#stopwatch_max_lbl").text() + 1
    var time = + $("#stopwatch_time_lbl").text()
    var xhr = new XMLHttpRequest();
    xhr.open("GET", swIP + "/update?skill="+max+"&timer="+time, true);
    xhr.send();
    $("#stopwatch_max_lbl").text(max)
    $("#stopwatch_time_lbl").text(time)
  }
}, false);
g.addEventListener('skill', function (e) {
  alert("hello")
}, false);
g.addEventListener('timer', function(e) {
  alert("hello")
  $("#gaurd_time_lbl").text(e.data)
}, false);
g.addEventListener('active', function(e) {
  if (e.data == 1) {
    $("#gCard").toggleClass("border-color-red")
  } else {
    $("#gCard").toggleClass("border-color-red")
  }
}, false);
rt.addEventListener('skill', function(e) {
  $("#tek_max_lbl").text(e.data)
}, false);
rt.addEventListener('timer', function(e) {
  $("#tek_time_lbl").text(e.data)
}, false);
rt.addEventListener('active', function(e) {
  if (e.data == 1) {
    $("#swCard").toggleClass("border-color-red")
  } else {
    $("#swCard").toggleClass("border-color-red")
  }
}, false);
swt.addEventListener('skill', function(e) {
  $("#switcher_max_lbl").text(e.data)
}, false);
swt.addEventListener('timer', function(e) {
  $("#switcher_time_lbl").text(e.data)
}, false);
swt.addEventListener('active', function(e) {
  if (e.data == 1) {
    $("#swtCard").toggleClass("border-color-red")
  } else {
    $("#swtCard").toggleClass("border-color-red")
  }
}, false);
w.addEventListener('skill', function(e) {
  $("#wise_max_lbl").text(e.data)
}, false);
w.addEventListener('timer', function(e) {
  $("#wise_time_lbl").text(e.data)
}, false);
w.addEventListener('active', function(e) {
  if (e.data == 1) {
    $("#wCard").toggleClass("border-color-red")
  } else {
    $("#wCard").toggleClass("border-color-red")
  }
}, false);
gj.addEventListener('skill', function(e) {
  $("#gadjet_max_lbl").text(e.data)
}, false);
gj.addEventListener('timer', function(e) {
  $("#gadjet_time_lbl").text(e.data)
}, false);

energy.addEventListener('volume', function(e) {
  mixerLoad('energy', e.data)
  var xhr = new XMLHttpRequest();
  if (e.data == energyVal[year_id] && notConf) {
    xhr.open("GET", energyIP + "/update?mode=1", true);
  }
  else if (e.data == energyVal[year_id] && !notConf) {
    xhr.open("GET", energyIP + "/update?mode=2", true);
  }
  else {
    xhr.open("GET", energyIP + "/update?mode=0", true);
  }
  xhr.send()
}, false);
gravity.addEventListener('volume', function(e) {
  mixerLoad('gravity', e.data)
  var xhr = new XMLHttpRequest();
  if (e.data == gravityVal[year_id] && notConf) {
    xhr.open("GET", gravityIP + "/update?mode=1", true);
  }
  else if (e.data == gravityVal[year_id] && !notConf) {
    xhr.open("GET", gravityIP + "/update?mode=2", true);
  }
  else {
    xhr.open("GET", gravityIP + "/update?mode=0", true);
  }
  xhr.send()
}, false);
speed.addEventListener('volume', function(e) {
  mixerLoad('speed', e.data)
  var xhr = new XMLHttpRequest();
  if (e.data == speedVal[year_id] && notConf) {
    xhr.open("GET", speedIP + "/update?mode=1", true);
  }
  else if (e.data == speedVal[year_id] && !notConf) {
    xhr.open("GET", speedIP + "/update?mode=2", true);
  }
  else {
    xhr.open("GET", speedIP + "/update?mode=0", true);
  }
  xhr.send()
}, false);
accel.addEventListener('volume', function(e) {
  mixerLoad('accel', e.data)
  var xhr = new XMLHttpRequest();
  if (e.data == accelVal[year_id] && notConf) {
    xhr.open("GET", accelIP + "/update?mode=1", true);
  }
  else if (e.data == accelVal[year_id] && !notConf) {
    xhr.open("GET", accelIP + "/update?mode=2", true);
  }
  else {
    xhr.open("GET", accelIP + "/update?mode=0", true);
  }
  xhr.send()
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
function checkYear(inp) {
  for (var i = 0; i < yearObj.length; i++) {
    if (inp >= yearObj[i]["start"] && inp < yearObj[i]["end"]) {
        audioElement.setAttribute('src', 'sound/' + yearObj[i]["sound"] + '.mp3');
        audioElement.play()
        $("#year_name_lbl").text(yearObj[i]["name"])
        return;
    }
  }
  $("#year_name_lbl").text("UNKNOWN")
  audioElement.setAttribute('src', 'sound/nothing.mp3');
}

year.addEventListener('move', function(e) {
  if (e.data == "1") {
    $("#exitBtn").addClass("btn-danger")
    $("#exitBtn").removeClass("btn-outline-danger")
    var e = false
    var g = false
    var s = false
    var a = false
    for (var i = 0; i < years.length; i++) {
      if ($("#year_lbl").text() == years[i]) {
        e = $("#energy_lbl").text() == energyVal[i]
        g = $("#gravity_lbl").text() == gravityVal[i]
        s = $("#speed_lbl").text() == speedVal[i]
        a = $("#accel_lbl").text() == accelVal[i]

        year_id = i
      }
    }
    if (e && g && s && a & notConf) {
      notConf = 0;
      var xhr1 = new XMLHttpRequest();
      var xhr2 = new XMLHttpRequest();
      var xhr3 = new XMLHttpRequest();
      var xhr4 = new XMLHttpRequest();
      xhr1.open("GET", energyIP + "/update?mode=2", true);
      xhr1.send()
      xhr2.open("GET", gravityIP + "/update?mode=2", true);
      xhr2.send()
      xhr3.open("GET", speedIP + "/update?mode=2", true);
      xhr3.send()
      xhr4.open("GET", accelIP + "/update?mode=2", true);
      xhr4.send()
    }
  }
  else {
    $("#exitBtn").addClass("btn-outline-danger")
    $("#exitBtn").removeClass("btn-danger")
    var xhr1 = new XMLHttpRequest();
    var xhr2 = new XMLHttpRequest();
    var xhr3 = new XMLHttpRequest();
    var xhr4 = new XMLHttpRequest();
    xhr1.open("GET", energyIP + "/update?mode=1", true);
    xhr1.send()
    xhr2.open("GET", gravityIP + "/update?mode=1", true);
    xhr2.send()
    xhr3.open("GET", speedIP + "/update?mode=1", true);
    xhr3.send()
    xhr4.open("GET", accelIP + "/update?mode=1", true);
    xhr4.send()
    notConf = 1;
    year_id = 4
  }

}, false);

year.addEventListener('mode', function(e) {
  if (e.data == 1) {

    lunchInterval = setInterval(lunch, 300)

    checkYear($("#year_lbl").text())

    audioElement.addEventListener('ended', function() {
        alert("finish")
    }, false);

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

function year_conf(inp) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", yearIP + "/update?year="+inp, true);
    xhr.send();
    $("#year_lbl").text(inp)
    checkYear(inp)
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
