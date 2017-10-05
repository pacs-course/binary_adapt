clear all
clc
filename = 'error_data'
delimiterIn = '\t';
headerlinesIn = 1;
A = importdata(filename,delimiterIn,headerlinesIn);
semilogy(A.data(:,1), A.data(:,2), 'linewidth', 2);
%title (A.textdata);
xlabel ('# iterations');
ylabel ('L^{2} error');
ylim([1E-16 1])

[garbage, expr] = strtok(A.textdata, ' ');
temp = strcat('@(x)', expr);
temp_2 = cell2mat(strrep(temp, '^', '.^'));
temp_3 = strrep(temp_2, '*', '.*');
f = str2func(temp_3);
x = linspace(0,1,1000);
figure
plot(x, f(x), 'linewidth', 2);
xlabel('x')
ylabel('f(x)')
