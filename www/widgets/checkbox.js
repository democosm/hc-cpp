//Checkbox class
class Checkbox
{
  constructor(elementname)
  {
    //Add properties
    this.widget = document.getElementById(elementname);
    this.elementname = elementname;

    //Check for widget not found
    if(this.widget == null)
      return;

//Create parent for widget
//var wrapper = document.createElement('div');
//this.widget.parentNode.insertBefore(wrapper, this.widget);
//wrapper.appendChild(this.widget); 

    //Add event listeners to widget if not read only
    if(!this.widget.readOnly)
    {
      this.widget.addEventListener("change", this.OnChange.bind(this));
    }
  }

  OnChange()
  {
    //Check for widget not found
    if(this.widget == null)
      return;

    //Encode element command as JSON
    var jsoncmd = JSON.stringify(["se",this.elementname,this.widget.checked]);
    console.log(jsoncmd);

    //Send HTTP request to server
    var req = new XMLHttpRequest();
    req.open("GET", "hcquery.php?cmd=[" + Xactnum + "," + jsoncmd + "]&" + (new Date()).getTime(), true);
    req.send(null);

    //Increment transaction number
    Xactnum++;

    //Limit 0-99
    if(Xactnum > 99)
      Xactnum = 0;
  }

  UpdateValue(elementname, elementval, err)
  {
    //Check for widget not found
    if(this.widget == null)
      return;

    //Check for not our element
    if(this.elementname !== elementname)
      return;

    //Update widget value
    if(elementval == "true")
      this.widget.checked = true;
    else
      this.widget.checked = false;

//This requires a parent node to contain the checkbox
    //Check for error
//    if(err == "None")
//      this.widget.parentNode.style.backgroundColor = "";
//    else
//      this.widget.parentNode.style.backgroundColor = "yellow";
  }
}
