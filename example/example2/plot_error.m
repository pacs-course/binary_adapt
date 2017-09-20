clear all
clc
filename = 'error_data_XSquared';
delimiterIn = '\t';
headerlinesIn = 1;
A = importdata(filename,delimiterIn,headerlinesIn);
semilogy(A.data(:,1), A.data(:,2), 'linewidth', 2);
%title (A.textdata);
xlabel ('# iterations');
ylabel ('L^{2} error');

%% modify this
f = @(x) x.^2;
x = linspace(0,1,1000);
figure
plot(x, f(x), 'linewidth', 2);
xlabel('x')
ylabel('f(x)')
