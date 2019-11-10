//Dropdown widget class
class DropdownWidget
{
  constructor(elementname)
  {
    //Add properties
    this.widget = document.getElementById(elementname);
    this.elementname = elementname;
    this.focused = false;

    //Check for widget not found
    if(this.widget == null)
      return;

    //Add event listeners to widget if not read only
    if(!this.widget.readOnly)
    {
      this.widget.addEventListener("focus", this.OnFocus.bind(this));
      this.widget.addEventListener("blur", this.OnBlur.bind(this));
      this.widget.addEventListener("change", this.OnChange.bind(this));
    }
  }

  OnFocus()
  {
    //Check for widget not found
    if(this.widget == null)
      return;

    //Make visual change and don't allow update of widget value
    this.widget.style.backgroundColor = "lightblue";
    this.focused = true;
  }

  OnBlur()
  {
    //Check for widget not found
    if(this.widget == null)
      return;

    //Make visual change and allow update of widget value
    this.widget.style.backgroundColor = "";
    this.focused = false;
  }

  OnChange()
  {
    //Check for widget not found
    if(this.widget == null)
      return;

    //Encode element command as JSON
    var jsoncmd = JSON.stringify(["se",this.elementname,this.widget.value]);

    //Send HTTP request to server
    var req = new XMLHttpRequest();
    req.open("GET", "hcquery.php?cmd=[" + Xactnum + "," + jsoncmd + "]&" + (new Date()).getTime(), true);
    req.send(null);

    //Increment transaction number
    Xactnum++;

    //Limit 0-99
    if(Xactnum > 99)
      Xactnum = 0;

    //Force call of blur event handler
    this.widget.blur();
  }

  UpdateValue(elementname, elementval, err)
  {
    //Check for widget not found
    if(this.widget == null)
      return;

    //Check for not our element
    if(this.elementname !== elementname)
      return;

    //Check for focused
    if(this.focused)
      return;

    //Update widget value
    this.widget.value = elementval;

    //Check for error
    if(err == "None")
      this.widget.style.backgroundColor = "";
    else
      this.widget.style.backgroundColor = "yellow";
  }
}
