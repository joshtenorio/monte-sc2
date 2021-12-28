#include "api.h"
#include "Ramp.h"

std::vector<Ramp> findRamps(){
    // get a list of points that are pathable but not buildable
    std::vector<sc2::Point2D> rampPoints;
    sc2::Point2D playableMin = gInterface->observation->GetGameInfo().playable_min;
    sc2::Point2D playableMax = gInterface->observation->GetGameInfo().playable_max;
    for(int x = playableMin.x; x < playableMax.x; x++){
        for(int y = playableMin.y; y < playableMax.y; y++){
            sc2::Point2D p = sc2::Point2D(x, y);
            if(gInterface->observation->IsPathable(p) && !gInterface->observation->IsPlacable(p)){
                rampPoints.emplace_back(p);
                sc2::Point3D p3d = sc2::Point3D(p.x, p.y, gInterface->observation->TerrainHeight(p));
                gInterface->debug->debugDrawTile(p3d, sc2::Colors::Teal);
            }
        }
    }
    

    // group together nearby ramp points to form a ramp
    std::vector<Ramp> ramps;
    while(!rampPoints.empty()){
        std::queue<sc2::Point2D> rampFrontier;
        rampFrontier.push(rampPoints.front());
        Ramp r;
        r.points.emplace_back(rampPoints.front());
        rampPoints.erase(rampPoints.begin());
        while(!rampPoints.empty() && !rampFrontier.empty()){
            sc2::Point2D r0 = rampFrontier.front();
            sc2::Point2D closestPoint = rampPoints.front();
            float distance = std::numeric_limits<float>::max();

            // get closest ramp point to rampfronter.front
            for(auto p : rampPoints){
                if(sc2::DistanceSquared2D(p, r0) < distance){
                    distance = sc2::DistanceSquared2D(p, r0);
                    closestPoint = p;
                }
            }

            if(distance >= 36){ // if point is too far away from ramp, disregard it
                rampFrontier.pop();
                continue;
            }

            rampFrontier.push(closestPoint);
            r.points.emplace_back(closestPoint);

            auto itr = std::find(rampPoints.begin(), rampPoints.end(), closestPoint);
            if(itr != rampPoints.end()) rampPoints.erase(itr);
        }

        // get center of ramp
        int x = 0, y = 0;
        int num = 0;
        for(auto& p : r.points){
            x += p.x;
            y += p.y;
            num++;
        }
        if(num)
            r.center = sc2::Point2D(x/num, y/num);
        
        ramps.emplace_back(r);
    } // while we still have ramp points to consider

    for(auto &r: ramps){
        sc2::Point3D p = API::toPoint3D(r.center);
        gInterface->debug->debugSphereOut(p, 8.0);
        // sort tiles by height in order of highest -> lowest
        int i = 1;
        while(i < r.points.size()){
            int j = i;
            while(j > 0 && API::toPoint3D(r.points[j-1]).z <= API::toPoint3D(r.points[j]).z){
                sc2::Point2D tmp = r.points[j-1];
                r.points[j-1] = r.points[j];
                r.points[j] = tmp;
                j -= 1;
            }
            i += 1;
        }
        gInterface->debug->debugDrawTile(API::toPoint3D(r.points[0]), sc2::Colors::Red);
        gInterface->debug->debugDrawTile(API::toPoint3D(r.points[1]), sc2::Colors::Red);
        r.direction = Monte::Vector2D(r.points.back(), r.points.front());
    }

    gInterface->debug->sendDebug();
    return ramps;
}

MainRamp generateMainRamp(Ramp r){
    MainRamp main;
    main.center = r.center;
    main.points = r.points;
    main.isMainRamp = true;
    main.direction = Monte::Vector2D(r.points.back(), r.points.front());

    if(main.direction.dx > 0 && main.direction.dy > 0){
        gInterface->debug->debugSphereOut(API::toPoint3D(main.center), 8, sc2::Colors::Blue);
    }
    else if(main.direction.dx < 0 && main.direction.dy > 0){
        gInterface->debug->debugSphereOut(API::toPoint3D(main.center), 8, sc2::Colors::Green);

    }
    else if(main.direction.dx < 0 && main.direction.dy < 0){
        gInterface->debug->debugSphereOut(API::toPoint3D(main.center), 8, sc2::Colors::Red);
        
    }
    else if(main.direction.dx > 0 && main.direction.dy < 0){
        gInterface->debug->debugSphereOut(API::toPoint3D(main.center), 8, sc2::Colors::White);
        if(main.points[0].y < main.points[1].y){
            main.supplyDepotPoints.emplace_back(sc2::Point2D(main.points[1].x, main.points[1].y-1));
            main.supplyDepotPoints.emplace_back(sc2::Point2D(main.points[0].x+2, main.points[0].y+1));
        }
        else {

        }
    }
    gInterface->debug->sendDebug();
    return main;
}

void testRampLocation(MainRamp* r){
    std::string map = gInterface->observation->GetGameInfo().map_name;
    sc2::Point2D start = gInterface->observation->GetStartLocation();
    if(map == "2000 Atmospheres AIE"){
        
    }
    else if(map == "Blackburn AIE"){

    }
    else if(map == "Jagannatha AIE"){
        
    }
    else if(map == "Lightshade AIE"){
        
    }
    else if(map == "Romanticide AIE"){
        
    }
    else if(map == "Oxide AIE"){
        
    }
}