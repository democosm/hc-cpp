//Add event listeners
document.getElementById('opener button').addEventListener('click', OnClickOpenerButton);

//Start polling status
PollStatus();

function OnClickOpenerButton()
{
  //Do AJAX HC call
  HCICall('/pulserelayhigh', 0);
}

function PollStatus()
{
  //Do AJAX HC get and display value
  HCGet('/temperature');

  //Poll again in 1 second
  setTimeout('PollStatus();', 1000);
}

function RenderData(data)
{
  //Get element and update
  document.getElementById('door state').innerHTML = data.val;
}

function HCGet(pid)
{
  //Get HTTP request object
  var req = new XMLHttpRequest();

  //Send get request
  req.open('GET', 'hcget.php?pid=' + pid, true);
  req.onload = OnLoad;
  req.send(null);

  function OnLoad()
  {
    //Parse result as JSON
    var data = JSON.parse(req.responseText);

    //Render data
    RenderData(data);
  }
}

function HCCall(pid)
{
  var req;

  //Get HTTP request object
  req = new XMLHttpRequest();

  //Send post request
  req.open('POST', 'hccall.php?pid=' + pid, true);
  req.send(null);
}

function HCICall(pid, eid)
{
  var req;

  //Get HTTP request object
  req = new XMLHttpRequest();

  //Send post request
  req.open('POST', 'hcicall.php?pid=' + pid + '&eid=' + eid, true);
  req.send(null);
}
