function preload() {
  document.body.lastChild.style.color="white";
  document.body.lastChild.style.fontSize="XXX-large";
  document.body.lastChild.style.textAlign="center";
  document.body.lastChild.style.top="50%";
  document.body.lastChild.style.left="40%";
  loadPageHead('main');
  LogoSphere=loadImage("pfp_sphere.png");
}

var LogoSphere;
var font;
var stars;

function setup() {
  document.body.insertBefore(document.body.lastChild,document.body.firstChild);//make the canvas the first thing in the doc
  createCanvas(windowWidth-18, windowHeight, WEBGL);
  print(windowWidth+" "+windowHeight);
  initializeSphere(100, 100);
  //print(width+" "+height);
  stars = new Array(100);
  //generate the stars for the 3D image
  for (let i=0; i<stars.length; i++) {
    stars[i]= new star(random(-width/2,width/2),random(-height/2,height/2));
  }
  genStars(200);//generate the Stars for the rest of the page
}

var rotation =100;
function draw() {  
  background(0);
  for (let i=0; i<stars.length; i++) {
    stars[i].draw();
  }
  ambientLight(120,120,120);
  directionalLight(255,255,255,-0.5,-0.5,1);
  directionalLight(120,120,120,-0.5,-0.5,1);
  rotateY(radians(rotation));

  textureSphere(300, 300, 300, LogoSphere);
  rotateY(radians(-rotation));
  rotation++;
  fill(0, 255, 0);
  textAlign(CENTER, CENTER);
  textSize(100);
  noLights();
}

function windowResized() {
  resizeCanvas(windowWidth-4, windowHeight-4);  
  for (let i=0; i<stars.length; i++) {
    stars[i]= new star(random(-width/2,width/2),random(-height/2,height/2));
  }
  print(windowWidth+" "+windowHeight);
}

class star{
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }
  
  draw(){
    fill(255);
    circle(this.x,this.y,1);
    //print(this.x+" "+this.y+" "+1);
  }
}


function initializeSphere(numPtsW,numPtsH_2pi) {

  // The number of points around the width and height
  numPointsW=numPtsW+1;
  numPointsH_2pi=numPtsH_2pi;  // How many actual pts around the sphere (not just from top to bottom)
  numPointsH=ceil(numPointsH_2pi/2)+1;  // How many pts from top to bottom (abs(....) b/c of the possibility of an odd numPointsH_2pi)

  coorX= new Array(numPointsW);   // All the x-coor in a horizontal circle radius 1
  coorY= new Array(numPointsH);   // All the y-coor in a vertical circle radius 1
  coorZ= new Array(numPointsW);   // All the z-coor in a horizontal circle radius 1
  multXZ= new Array(numPointsH);  // The radius of each horizontal circle (that you will multiply with coorX and coorZ)

  for (let i=0; i<numPointsW; i++) {  // For all the points around the width
    let thetaW=i*2*PI/(numPointsW-1);
    coorX[i]=sin(thetaW);
    coorZ[i]=cos(thetaW);
  }

  for (let i=0; i<numPointsH; i++) {  // For all points from top to bottom
    if (int(numPointsH_2pi/2) != numPointsH_2pi/2 && i==numPointsH-1) {  // If the numPointsH_2pi is odd and it is at the last pt
      let thetaH=(i-1)*2*PI/(numPointsH_2pi);
      coorY[i]=cos(PI+thetaH);
      multXZ[i]=0;
    } else {
      //The numPointsH_2pi and 2 below allows there to be a flat bottom if the numPointsH is odd
      let thetaH=i*2*PI/(numPointsH_2pi);

      //PI+ below makes the top always the point instead of the bottom.
      coorY[i]=cos(PI+thetaH);
      multXZ[i]=sin(thetaH);
    }
  }
}

function textureSphere( rx,  ry,  rz,  t) { 
  // These are so we can map certain parts of the image on to the shape 
  var changeU=t.width/(float)(numPointsW-1); 
  var changeV=t.height/(float)(numPointsH-1); 
  var u=0;  // Width variable for the texture
  var v=0;  // Height variable for the texture
  //CBiSphere= createShape();

  beginShape(TRIANGLE_STRIP);
  noStroke();
  texture(t);
  for (let i=0; i<(numPointsH-1); i++) {  // For all the rings but top and bottom
    // Goes into the array here instead of loop to save time
    var coory=coorY[i];
    var cooryPlus=coorY[i+1];

    var multxz=multXZ[i];
    var multxzPlus=multXZ[i+1];

    for (let j=0; j<numPointsW; j++) { // For all the pts in the ring
      normal(-coorX[j]*multxz, -coory, -coorZ[j]*multxz);
      vertex(coorX[j]*multxz*rx, coory*ry, coorZ[j]*multxz*rz, u, v);
      normal(-coorX[j]*multxzPlus, -cooryPlus, -coorZ[j]*multxzPlus);
      vertex(coorX[j]*multxzPlus*rx, cooryPlus*ry, coorZ[j]*multxzPlus*rz, u, v+changeV);
      u+=changeU;
    }
    v+=changeV;
    u=0;
  }
  endShape();
}
