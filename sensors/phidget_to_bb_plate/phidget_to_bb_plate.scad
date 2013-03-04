// units are in mm

// phidget mounting hole dimensions
ph=108.001;
pw=28.9814;
pr=1.6375;	

// bb mounting hole dimensions
// must be > 117 (pboard h)
bh=150;
bw=25;
br=3.3;

// total size dimensions
th=175.5;
tw=37.5;


module holes(w, h, r) {
	translate([-w/2, -h/2])
	circle(r, center=true, $fn=detail);
	translate([w/2, -h/2])
	circle(r, center=true, $fn=detail);
	translate([w/2, h/2])
	circle(r, center=true, $fn=detail);
	translate([-w/2, h/2])
	circle(r, center=true, $fn=detail);
}

difference() {
	//base
	square([tw, th], center=true);

	// mounting holes	
	holes(pw, ph, pr);
	holes(bw, bh, br);
}

