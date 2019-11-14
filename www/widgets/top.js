//Global variables
var Xactnum = 0;
var Widgets =
[
  //Add save state widget handling
//  new SaveWidget("SaveState"),

  //Add server widget handling
  new InputWidget("/.server/name"),
  new InputWidget("/.server/version"),
  new InputWidget("/.server/infofilecrc"),
  new InputWidget("/.server/senderrcnt"),
  new InputWidget("/.server/recverrcnt"),
  new InputWidget("/.server/deserrcnt"),
  new InputWidget("/.server/cellerrcnt"),
  new InputWidget("/.server/opcodeerrcnt"),
  new InputWidget("/.server/piderrcnt"),
  new InputWidget("/.server/interrcnt"),
  new InputWidget("/.server/goodxactcnt"),

  //Add boolean widgets
  new Checkbox("/bool/val"),
  new DropdownWidget("/bool/vale"),

  //Add uint32 widget
  new InputWidget("/uint32/val"),
  new DropdownWidget("/uint32/vale"),
];

//Start polling for status
PollStatus();

function PollStatus()
{
  function OnLoad()
  {
    //Check for no communication with server
    if(req.responseText.includes("Timed out waiting for response"))
    {
      //Set background color to indicate error
      document.body.style.background = "yellow";
      return;
    }

    //Reset to default background color
    document.body.style.background = "";

    //Parse response as JSON
    var data = JSON.parse(req.responseText);

    //Loop through each cell in response
    for(var i in data)
    {
      //Give each widget a chance to process response cell
      for(var w in Widgets)
      {
        Widgets[w].UpdateValue(data[i][1], data[i][2], data[i][3]);
      }
    }
  }

  //Send get command to request status of widget elements
  var req = new XMLHttpRequest();
  var cmd = [Xactnum];
  var i = 1;
  for(var w in Widgets)
  {
    if(Widgets[w].widget != null)
      cmd[i++] = ["ge",Widgets[w].elementname];
  }
  req.open("GET", "hcquery.php?cmd=" + JSON.stringify(cmd) + "&" + (new Date()).getTime(), true);
  req.onload = OnLoad;
  req.send(null);

  //Increment transaction number
  Xactnum++;

  //Limit 0-99
  if(Xactnum > 99)
    Xactnum = 0;

  //Start the polling again after time has elapsed
  setTimeout("PollStatus();", 1000);
}
