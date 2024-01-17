window.loadPageHead = function loadPageHead(page){
    document.querySelector('.head').outerHTML = getHeader(page)
}

function getHeader(page){
  return '<div class="head">'+
  '<table border=3 cellspacing=10 cellpadding=3 class="border">'+
    '<tr>'+
      '<td class="head"'+ ((page=='main') ? 'style="background-color: #00e7ff47;"': "" )+'><a href="index.html" class="head">Home</a></td>'+
      '<td class="head"'+ ((page=='homework') ? 'style="background-color: #00e7ff47;"': "" )+'><a href="homework.html" class="head">Home Work</a></td>'+
    '</tr>'+
  '</table>'+
'</div>'
}

function genStars(num){
  var md = document.createElement('div');
  for(var i=0;i<num;i++){
    var element = document.createElement('div');
    element.style.width="1px";
    element.style.height="1px";
    element.style.backgroundColor="white";
    element.style.position="absolute";
    element.style.top=Math.floor(Math.random()*1000000%getPageHeight())+"px";
    element.style.left=Math.floor(Math.random()*1000000%getPageWidth())+"px";
    element.style.zIndex=-1;
    md.appendChild(element);
  }
  document.body.appendChild(md);
}


function getPageWidth() {
    return document.body.scrollWidth
  }
  
  function getPageHeight() {
    return  document.body.scrollHeight
  }
