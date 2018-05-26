
translate([0,0,0]) {
difference() {
        techo();
  
    union() {
        translate([50,50,4.3]) {
            ml5811();
        }
        
        translate([100,50,4.5]) {
            bh1750();
        }
    }
}
}

*estructura();
*soporte();
rotate(a = 90, v = [0,1,0]) {
  * puerta();
}
*lama();


module estructura() {
    difference() {
        // Base y 4 columnas laterales
        union() {
            cube([150,150,5]);
        
            columna(180);
    
            translate ([130,0,0]) {
                columna(-90);
            }
        
            translate ([0,130,0]) {
                columna(90);
            }
    
            translate ([130,130,0]) {
                columna(0);
            }   
        }
        
        translate([130,20,0]) {
                cylinder(h=20,r=3.25);
        }
    }
}

module soporte() {
    translate([20,50,5]) {
        cube([2.5,50,8]);
    }
    translate([60,50,5]) {
        cube([4,50,8]);
    }
    translate([128.4,50,5]) {
        cube([2.5,50,8]);
    }
}

// Recorte techo
module recorteTecho(a) {
    rotate_about_pt(a, [0, 0, 1], [10, 10, 0]) {
    union() {
        translate ([4.75,4.75,0]) {
            cube([5.5,5.5,7]);
        }

        translate ([0,20,0]) {
            union() {
                cube([3,110,7]);
                   
                translate ([3,-3,0]) {
                    cube([3,116,7]);
                }
            }
        }
    }
    }
}


// Techo
module techo() {
    difference() {
        cube([150,150,6]);
        recorteTecho(0);
        
        translate([130.01,130,0]) {
            recorteTecho(180);
        }
        
        translate([0,130,0]) {
            recorteTecho(-90);
        }       
   
        translate([130,-0.01,0]) {
            recorteTecho(90);
        }        
    }
}

// Columna
module columna (a) {
    rotate_about_pt(a, [0, 0, 1], [10, 10, 0]) {
        difference() {
            union() {
                    translate ([0,0,4]) {
                        cube([20,20,140]);
                    }

                    translate ([10,10,144]) {
                        cube([5,5,6]);
                    }
            }
            union() {    
                cube([10,10,150]);
                
                translate ([14,0,0]) {
                    cube([3,3,150]);
                }
                
                translate ([0,14,0]) {
                    cube([3,3,150]);
                }    
            }
        }
    }
}

module puerta () {
    //Puerta
    translate ([0,20,5]) {
        difference() {
            union() {
                cube([4,110,145]);
                   
                translate ([3,-3,0]) {
                    cube([2,116,145]);
                }
            }
            translate ([0,10,12.5]) {
                cube([10,90,120]);
            }
        }
        
        for (i = [10:6:130]) {
            translate ([0,10,i]) {
                lama();
            }        
        }
    }
}

// Lama
module lama() {
CubePoints = [
  [  0,   0,  0 ],  //0
  [  5,   0,  2 ],  //1
  [  5,  90,  2 ],  //2
  [  0,  90,  0 ],  //3
  [  0,   0,  2 ],  //4
  [  5,   0,  5 ],  //5
  [  5,  90,  5 ],  //6
  [  0,  90,  2 ]]; //7
  
CubeFaces = [
  [0,1,2,3],  // bottom
  [4,5,1,0],  // front
  [7,6,5,4],  // top
  [5,6,2,1],  // right
  [6,7,3,2],  // back
  [7,4,0,3]]; // left
  
polyhedron( CubePoints, CubeFaces );
}

// ML5811
module ml5811() {
    difference() {
        union() {
            cube([15.1,15.1,1.8]);
            translate([0,15,-10.1]) {
                cube([15.1,3,12.2]);
            }
            translate([5.5,4,0]) {
                cube([3.6,4,2.2]);
            }
        }
        union() {
            translate([3.1/2+0.8,2.55]) {
                cylinder(h=2, r=1.2, $fn=100);
            }
            
            translate([12.75,2.55]) {
                cylinder(h=2, r=1.2, $fn=100);
            }
        }
    }
}

// BH1750
module bh1750() {
    difference() {
        union() {
            cube([13.8,18.3,1.5]);
            translate([0,18.3,-10.1]) {
                cube([13.8,3,12.2]);
            }
            translate([5.7,6.6,0]) {
                cube([2.4,1.4,2.2]);
            }
        }
        
        union() {
            translate([2.5,2.5]) {
                cylinder(h=2, r=1.2, $fn=100);
            }
            
            translate([11.3,2.5]) {
                cylinder(h=2, r=1.2, $fn=100);
            }
        }
    }
}


module rotate_about_pt(a, v, pt) translate(pt) rotate(a,v)
translate(-pt) children();
