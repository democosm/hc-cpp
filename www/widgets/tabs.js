function ShowTabPane(parent, tabname)
{
  var index;

  //Hide all tab panes
  for(index=0; index<parent.childNodes.length; index++)
    if(parent.childNodes[index].className != null)
      if(parent.childNodes[index].className === "tabpane")
        parent.childNodes[index].style.display = "none";

  //Show tab pane with matching name
  for(index=0; index<parent.childNodes.length; index++)
    if(parent.childNodes[index].id != null)
      if(parent.childNodes[index].id === tabname)
        parent.childNodes[index].style.display = "block";
};

function ActivateTabLink(parent, tablink)
{
  var index;

  //Deactivate all tab links
  for(index=0; index<parent.childNodes.length; index++)
    if(parent.childNodes[index].className != null)
      if(parent.childNodes[index].className.startsWith("tablink"))
        parent.childNodes[index].className = parent.childNodes[index].className.replace(" active", "");

  //Activate specified tab link
  tablink.className += " active";
}

function OpenTab(evt, tabname)
{
  var i;
  var tablinks;

  //Show tab pane with the specified name (hide others)
  ShowTabPane(evt.target.parentElement.parentElement, tabname);

  //Activate tab link (hide others)
  ActivateTabLink(evt.target.parentElement, evt.target);
}
