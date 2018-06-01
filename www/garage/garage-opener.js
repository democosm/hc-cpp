PollStatus();

document.getElementById("door1 button").addEventListener("click", OnClickDoor1Button);
document.getElementById("door2 button").addEventListener("click", OnClickDoor2Button);

function OnClickDoor1Button()
{
  var params = [["/pulserelayhigh",0]];
  HCICall(params);
}

function OnClickDoor2Button()
{
  var params = [["/pulserelayhigh",1]];
  HCICall(params);
}

function PollStatus()
{
  var params = ["/temperature", "/cpuutilization"];
  HCGet(params);
  setTimeout("PollStatus();", 5000);
}

function RenderData(data)
{
  for(var i in data)
  {
    if(data[i][0] == "/temperature")
      document.getElementById("temperature").innerHTML = "Temperature: " + data[i][1] + " C";
    else if(data[i][0] == "/cpuutilization")
      document.getElementById("cpuutilization").innerHTML = "CPU Utilization: " + data[i][1] + "%";
  }
}

function HCGet(params)
{
  var req = new XMLHttpRequest();
  req.open("GET", "hcget.php?params=" + JSON.stringify(params), true);
  req.onload = OnLoad;
  req.send(null);

  function OnLoad()
  {
    var data = JSON.parse(req.responseText);
    RenderData(data);
  }
}

function HCCall(params)
{
  var req = new XMLHttpRequest();
  req.open("GET", "hccall.php?params=" + JSON.stringify(params), true);
  req.send(null);
}

function HCICall(params)
{
  var req = new XMLHttpRequest();
  req.open("GET", "hcicall.php?params=" + JSON.stringify(params), true);
  req.send(null);
}
