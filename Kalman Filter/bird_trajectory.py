'''
Team Members -
Ayush Agrawal B14CS010
Varun Kumar B14CS039
'''
from numpy import *
from numpy.linalg import inv, det
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# function for prediction
def predict(mean, cov, noise_mat, noise_cov):
    p_mean = dot(noise_mat, mean)
    p_cov = dot(noise_mat, dot(cov, noise_mat.T)) + noise_cov
    return(p_mean,p_cov)

# function for estimation   
def update(p_mean, p_cov, measured, measured_mat, measured_cov): 
    p_observed_mean = dot(measured_mat, p_mean) 
    p_observed_cov = measured_cov + dot(measured_mat, dot(p_cov, measured_mat.T)) 
    K_gain = dot(p_cov, dot(measured_mat.T, inv(p_observed_cov))) 
    e_mean = p_mean + dot(K_gain, (measured-p_observed_mean)) 
    e_cov = p_cov - dot(K_gain, dot(measured_mat, p_cov)) 
    return (e_mean,e_cov)

# number of data points
iteration = 100   # change the value to change the number of data points

# defining trajectory of the bird
theta = linspace(-4 * pi, 4 * pi, iteration)
z = linspace(-2, 2, iteration)
r = z**2 + 1
x = r * sin(theta)
y = r * cos(theta)

# prior model matrices
mean = array([[0.0], [0.0], [0.0], [0.1], [0.1], [0.1] ])
cov = diag((0.01, 0.01, 0.01, 0.01, 0.01, 0.01))

# transition model matrices
noise_factor = 0.1    # change the value to change noise matrix
noise_mat = array([[1, 0, noise_factor , 0, 0, 0], [0, 1, 0, noise_factor, 0, 0], [0, 0, 1, 0, 0, 0], [0, 0, 0,1, 0, 0], [0, 0, noise_factor,0, 1, 0], [noise_factor, 0, 0,0, 0, 1]])
noise_cov = eye(mean.shape[0])

# sensor model matrices
deviation = 1     # change the value to change covariance matrix
measured = array([[x[0]], [y[0]], [z[0] ]])
measured_mat = array([[1, 0, 0, 0, 0, 0], [0, 1, 0, 0, 0, 0], [0, 0, 1, 0, 0, 0]])
measured_cov = deviation*eye(measured.shape[0])

# arrays for plotting
# actual points
x1 = []
y1 = []
z1 = []
# estimated points
x2 = []
y2 = []
z2 = []
# predicted points
x3 = []
y3 = []
z3 = []

# kalman filter for 'iteration' data points
for i in range(1,iteration):
  (p_mean, p_cov) = predict(mean, cov, noise_mat, noise_cov)
  (e_mean, e_cov) = update(p_mean, p_cov, measured, measured_mat, measured_cov)
  # adding points
  x1.append(measured[0,0])
  y1.append(measured[1,0])
  z1.append(measured[2,0])
  x2.append(e_mean[0,0])
  y2.append(e_mean[1,0])
  z2.append(e_mean[2,0])
  x3.append(p_mean[0,0])
  y3.append(p_mean[1,0])
  z3.append(p_mean[2,0])
  measured = array([[x[i] + abs(0.1 * random.randn(1)[0])],[y[i] + abs(0.1 * random.randn(1)[0])], [z[i] + abs(0.1 * random.randn(1)[0])]])
  (mean,cov) = (e_mean, e_cov)

# plotting
fig = plt.figure()
ax = fig.gca(projection='3d')
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.plot(x1, y1, z1, label='measured',color = 'r')
ax.plot(x2, y2, z2, label='estimated', color= 'g')
#ax.plot(x3, y3, z3, label='predicted')     # uncomment to see the plot of predicted means
ax.legend()
plt.show()