browserName = navigator.appName;
browserVer = parseInt(navigator.appVersion);
        if (browserName == "Netscape" && browserVer >= 3) version = "n3";
        else version = "n3";
          
 
function img_act(imgName) {
             if (version == "n3") {
             imgOn = eval(imgName + "on.src");
             document [imgName].src = imgOn;
             }
     }
     function img_inact(imgName) {
             if (version == "n3") {
             imgOff = eval(imgName + "off.src");
             document [imgName].src = imgOff;
             }
      }
if (version == "n3") {
             toc1on = new Image(42, 197);
             toc1on.src = "./menu-images/home_on.gif";
             toc2on = new Image(42, 197);
             toc2on.src = "./menu-images/info_on.gif";
             toc3on = new Image(42, 197);
             toc3on.src = "./menu-images/download_on.gif";
             toc4on = new Image(42, 197);
             toc4on.src = "./menu-images/purchase_on.gif";
             toc5on = new Image(42, 197);
             toc5on.src = "./menu-images/support_on.gif";
             toc6on = new Image(42, 197);
             toc6on.src = "./menu-images/forum_on.gif";
     

 
             toc1off = new Image(42, 197);
             toc1off.src = "./menu-images/home_off.gif";
             toc2off = new Image(42, 197);
             toc2off.src = "./menu-images/info_off.gif";
             toc3off = new Image(42, 197);
             toc3off.src = "./menu-images/download_off.gif";
             toc4off = new Image(42, 197);
             toc4off.src = "./menu-images/purchase_off.gif";
             toc5off = new Image(42, 197);
             toc5off.src = "./menu-images/support_off.gif";
             toc6off = new Image(42, 197);
             toc6off.src = "./menu-images/forum_off.gif";
    
       
             }
function message (txt) {
window.status = txt;
setTimeout ("remove()",2500);
}
 
function remove() {
window.status = "";
}
