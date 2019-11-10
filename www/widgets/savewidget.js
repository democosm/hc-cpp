//Save state widget class
class SaveWidget
{
  constructor(elementname)
  {
    //Add properties
    this.widget = document.getElementById(elementname);
    this.elementname = elementname;

    //Check for widget not found
    if(this.widget == null)
      return;

    //Add event listeners to widget
    this.widget.addEventListener("click", this.OnClick.bind(this));
  }

  OnClick()
  {
    //Send HTTP request to server
    var req = new XMLHttpRequest();
    req.open("GET", "hcquery.php?cmd=[" + Xactnum + "," + JSON.stringify(["sa"]) + "]&" + (new Date()).getTime(), true);
    req.send(null);

    //Increment transaction number
    Xactnum++;

    //Limit 0-99
    if(Xactnum > 99)
      Xactnum = 0;
  }

  UpdateValue(elementname, elementval, err)
  {
    //Do nothing
  }
}
