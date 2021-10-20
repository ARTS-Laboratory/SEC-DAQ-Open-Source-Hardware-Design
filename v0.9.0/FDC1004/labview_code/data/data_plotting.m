clear all
close all
clc


d1 = load('cap_daq_1_002.lvm');
d2 = load('cap_daq_2_002.lvm');
d3 = load('rsg_002.lvm');

t1 = d1(:,1)/1e6;
t2 = d2(:,1)/1e6;
t3 = d3(:,1);

X = linspace(0,8000)
figure()
plot(X,t1(1:length(X)))
hold all
plot(X,t2(1:length(X)),'k')
plot(X,t3(1:length(X)),'g')
