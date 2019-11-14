PollStatus();

document.getElementById("door1 button").addEventListener("click", OnClickDoor1Button);
document.getElementById("door2 button").addEventListener("click", OnClickDoor2Button);

function OnClickDoor1Button()
{
  HCICall(0);
}

function OnClickDoor2Button()
{
  HCICall(1);
}

function PollStatus()
{
  HCGet();
  setTimeout("PollStatus();", 5000);
}

function RenderData(data)
{
  for(var i in data)
  {
    if(data[i][1] == "/temperature")
      document.getElementById("temperature").innerHTML = "Temperature: " + data[i][2] + " C";
    else if(data[i][1] == "/cpuutilization")
      document.getElementById("cpuutilization").innerHTML = "CPU Utilization: " + data[i][2] + "%";
  }
}

function HCGet()
{
  var req = new XMLHttpRequest();

  let cmd = [0,["ge","/temperature"],["ge","/cpuutilization"]];

  req.open("GET", "hcquery.php?cmd=" + JSON.stringify(cmd), true);
  req.onload = OnLoad;
  req.send(null);

  function OnLoad()
  {
    var data = JSON.parse(req.responseText);
    RenderData(data);
  }
}

function HCICall(relay)
{
  var req = new XMLHttpRequest();

  let cmd = [0,["ic","/pulserelayhigh",relay]];

  req.open("GET", "hcquery.php?cmd=" + JSON.stringify(cmd), true);
  req.send(null);
}
