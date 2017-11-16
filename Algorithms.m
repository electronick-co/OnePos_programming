%% Clear all and add the variables

clear all
load('RecognitionRoute1.mat');

%% Plot route with different average windows

InitOfCorrectData = 5;

windowSize = 5;
b = (1/windowSize)*ones(1,windowSize);
a = 1;

y5 = filter(b,a,y_calculated(InitOfCorrectData:end));
x5 = filter(b,a,x_calculated(InitOfCorrectData:end));
x5 = x5(windowSize:end); 
y5 = y5(windowSize:end); 


windowSize = 10;
b = (1/windowSize)*ones(1,windowSize);
a = 1;

y10 = filter(b,a,y_calculated(InitOfCorrectData:end));
x10 = filter(b,a,x_calculated(InitOfCorrectData:end));
x10 = x10(windowSize:end); 
y10 = y10(windowSize:end); 

rssi1_filt = filter(b,a,rssi1(InitOfCorrectData:end));
rssi2_filt = filter(b,a,rssi2(InitOfCorrectData:end));
rssi3_filt = filter(b,a,rssi3(InitOfCorrectData:end));
rssi4_filt = filter(b,a,rssi4(InitOfCorrectData:end));

rssi1_filt = rssi1_filt(windowSize:end);
rssi2_filt = rssi2_filt(windowSize:end);
rssi3_filt = rssi3_filt(windowSize:end);
rssi4_filt = rssi4_filt(windowSize:end);

windowSize = 20;
b = (1/windowSize)*ones(1,windowSize);
a = 1;

y20 = filter(b,a,y_calculated(InitOfCorrectData:end));
x20 = filter(b,a,x_calculated(InitOfCorrectData:end));
x20 = x20(windowSize:end); 
y20 = y20(windowSize:end);

plot(x_calculated(InitOfCorrectData:end), y_calculated(InitOfCorrectData:end))
hold on
%plot room
line([-70,350],[1,1],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,-70],[1,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,100],[250,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,100],[250,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,432],[281,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,432],[281,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,495],[110,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([495,495],[110,0],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
viscircles([432,200],5) %node 1
viscircles([0,0],5) %node 2
viscircles([495,5],5) %node 3
viscircles([0,250],5) %node 4

figure
plot(x5,y5)
hold on
%plot room
line([-70,350],[1,1],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,-70],[1,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,100],[250,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,100],[250,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,432],[281,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,432],[281,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,495],[110,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([495,495],[110,0],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
viscircles([432,200],5) %node 1
viscircles([0,0],5) %node 2
viscircles([495,5],5) %node 3
viscircles([0,250],5) %node 4

figure
plot(x10,y10)
hold on
%plot room
line([-70,350],[1,1],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,-70],[1,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,100],[250,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,100],[250,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,432],[281,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,432],[281,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,495],[110,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([495,495],[110,0],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
viscircles([432,200],5) %node 1
viscircles([0,0],5) %node 2
viscircles([495,5],5) %node 3
viscircles([0,250],5) %node 4

figure
plot(x20,y20)
hold on
%plot room
line([-70,350],[1,1],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,-70],[1,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,100],[250,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,100],[250,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,432],[281,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,432],[281,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,495],[110,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([495,495],[110,0],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
viscircles([432,200],5) %node 1
viscircles([0,0],5) %node 2
viscircles([495,5],5) %node 3
viscircles([0,250],5) %node 4

%% Plot RSSI signals

figure
plot(rssi1_filt)
hold on
plot(rssi2_filt)
hold on
plot(rssi3_filt)
hold on
plot(rssi4_filt)

figure
plot(rssi1)
hold on
plot(rssi2)
hold on
plot(rssi3)
hold on
plot(rssi4)

%% Trilateration Test

%Calcultes distance using  formula
txPower = -51;
d_signal_1 = 10.^( (txPower - rssi1_filt)/20);
d_signal_2 = 10.^( (txPower - rssi2_filt)/20);
d_signal_3 = 10.^( (txPower - rssi3_filt)/20);
d_signal_4 = 10.^( (txPower - rssi4_filt)/20);

func = fittype('sqrt( (x-x0)^2 + (y-y0)^2 )');

%% Fingerprinting

t1 = table(x10,y10,rssi1_filt);
model_rssi1 = fitrgp(t1,'rssi1_filt');

t2 = table(x10,y10,rssi2_filt);
model_rssi2 = fitrgp(t2,'rssi2_filt');

t3 = table(x10,y10,rssi3_filt);
model_rssi3 = fitrgp(t3,'rssi3_filt');

t4 = table(x10,y10,rssi4_filt);
model_rssi4 = fitrgp(t4,'rssi4_filt');

%% HeatMap
clear heat_rssi1

cell_dimention = 5;
room_x = linspace(0,500, round(500/cell_dimention));
room_y = linspace(0,250, round(250/cell_dimention)); 

heat_rssi1 =  zeros(size(room_x,2),size(room_y,2));
heat_rssi2 =  zeros(size(room_x,2),size(room_y,2));
heat_rssi3 =  zeros(size(room_x,2),size(room_y,2));
heat_rssi4 =  zeros(size(room_x,2),size(room_y,2));

for i=1:size(heat_rssi1,1)
    for j = 1:size(heat_rssi1,2)
        heat_rssi1(i,j) = model_rssi1.predict([room_x(i),room_y(j)]);
        heat_rssi2(i,j) = model_rssi2.predict([room_x(i),room_y(j)]);
        heat_rssi3(i,j) = model_rssi3.predict([room_x(i),room_y(j)]);
        heat_rssi4(i,j) = model_rssi4.predict([room_x(i),room_y(j)]);
    end
end

%%

[X,Y] = meshgrid(room_x',room_y');

h = surf(X,Y,heat_rssi1')
view(0,90)
shading interp
hold on
z = get(h,'ZData');
set(h,'ZData',z-10)  
z_max = max(max(get(h,'Zdata')));

%plot room
line([-70,350],[1,1],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,-70],[1,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,100],[250,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,100],[250,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,432],[281,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,432],[281,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,495],[110,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([495,495],[110,0],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
viscircles([432,200],5) %node 1
viscircles([0,0],5) %node 2
viscircles([495,5],5) %node 3
viscircles([0,250],5) %node 4

figure

surf(X,Y,heat_rssi2')
view(0,90)
shading interp
line([-70,350],[1,1],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,-70],[1,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,100],[250,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,100],[250,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,432],[281,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,432],[281,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,495],[110,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([495,495],[110,0],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
viscircles([432,200],5) %node 1
viscircles([0,0],5) %node 2
viscircles([495,5],5) %node 3
viscircles([0,250],5) %node 4

figure

surf(X,Y,heat_rssi3')
view(0,90)
shading interp
line([-70,350],[1,1],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,-70],[1,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,100],[250,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,100],[250,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,432],[281,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,432],[281,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,495],[110,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([495,495],[110,0],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
viscircles([432,200],5) %node 1
viscircles([0,0],5) %node 2
viscircles([495,5],5) %node 3
viscircles([0,250],5) %node 4

figure

surf(X,Y,heat_rssi4')
view(0,90)
shading interp
line([-70,350],[1,1],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,-70],[1,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([-70,100],[250,250],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,100],[250,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([100,432],[281,281],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,432],[281,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([432,495],[110,110],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
line([495,495],[110,0],[z_max,z_max], 'LineWidth',3, 'Color', 'k')
viscircles([432,200],5) %node 1
viscircles([0,0],5) %node 2
viscircles([495,5],5) %node 3
viscircles([0,250],5) %node 4

%% test to Estimate distance with RSSI data

distance = zeros(size(heat_rssi1));
% 80,150,355,171,439,199,-54,-61,-59,-57
M = [-54,-61,-59,-57];

distance= sqrt( ( (heat_rssi1 - M(1)).^2 + (heat_rssi2 - M(2)).^2 + (heat_rssi3 - M(3)).^2 + (heat_rssi4 - M(4)).^2 )/4  );

surf(X,Y,distance')
view(0,90)
shading interp

[a,b] = find(distance == min(distance(:)));
x_current = a*cell_dimention
y_current = b*cell_dimention


%% Now MACHINEEEE LEARNIIIINGGUUU

%Prepare the data to be entered on the algorithm

ml_cell_dimention = 80; %cm
class = zeros(size(x10));

x_ml = ceil(x10/ml_cell_dimention);
y_ml = ceil(y10/ml_cell_dimention);
max_col = ceil(495/ml_cell_dimention);

class = (y_ml -1)*max_col + x_ml;

table_ML = table(rssi1_filt,rssi2_filt,rssi3_filt,rssi4_filt,class);


%% SECOND WALK PHASE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

