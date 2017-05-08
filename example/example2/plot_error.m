filename = 'error_data';
delimiterIn = '\t';
headerlinesIn = 1;
A = importdata(filename,delimiterIn,headerlinesIn);
semilogy(A.data(:,1), A.data(:,2));
title (A.textdata);
xlabel ('# iterations');
ylabel ('L2 error');