import numpy as np
from scipy.spatial import distance
from scipy.sparse import csgraph
from scipy.sparse import csr_matrix
import turtle
import math


class Movement:
    angle = None

    def __init__(self, mode, overrided_functions):
        self.angle = 0.0
        if mode == 'simulation':
            self.turtle = turtle.Turtle()
            self.turtle.reset()
            self.turtle.speed(1)
            self.setpos = self.turtle.setpos
            self.left = self.turtle.left
            self.right = self.turtle.right
            self.forward = self.turtle.forward
            self.penup = self.turtle.penup
            self.pendown = self.turtle.pendown
        elif mode == 'free':
            # Las funciones necesitan los mismos parametros, en el mismo orden
            self.setpos = overrided_functions[0]
            self.left = overrided_functions[1]
            self.right = overrided_functions[2]
            self.forward = overrided_functions[3]
            self.penup = overrided_functions[4]
            self.pendown = overrided_functions[5]
        else:
            raise NotImplementedError


class Painter2:
    points = None
    connection_matrix = None
    dist_matrix = None
    dist_matrix2 = None
    predec_matrix = None
    objs_with_movement = []

    def __init__(self, mode, num__bots, filenames=None, overrided_functions=None):
        """
        :param filenames: 2-tuple (filename1, filename2)
        Painter('free', 3, [make_this, make_that, go_to])
        """
        self.N_BOTS = num__bots
        self.MODE = mode
        self._OVERRIDED_FUNCTIONS = overrided_functions
        if filenames is not None:
            self.is_filename_in_constr = True
            self.read_paint(filenames[0], filenames[1])
        else:
            self.is_filename_in_constr = False

    def show_paint(self):
        def move(x, y):
            turtle.penup()
            turtle.setpos(x, y)
            turtle.pendown()

        turtle.reset()
        turtle.speed(0)  # Aunque sea 0 es la velocidad más rápida, sin animación
        turtle.hideturtle()
        for i, point in enumerate(self.points):
            move(int(point[0]), int(point[1]))
            for j in range(len(self.points)):
                if self.connection_matrix[i][j] == 1:
                    neighbor_point = self.points[j]
                    turtle.setpos(int(neighbor_point[0]), int(neighbor_point[1]))
                    move(int(point[0]), int(point[1]))

    def read_paint(self, points_filename, connect_filename):
        with open(points_filename) as points_file:
            self.points = []
            for line in points_file:
                line = line.split()
                self.points.append((np.float64(line[1]), np.float64(line[2])))
        with open(connect_filename) as connect_file:
            self.connection_matrix = np.zeros((len(self.points), len(self.points)), dtype=np.int)
            self.n_edges = 0
            for line in connect_file:
                line = line.split()
                index_vertex1 = int(line[0])
                index_vertex2 = int(line[1])
                self.connection_matrix[index_vertex1 - 1, index_vertex2 - 1] = 1
                self.connection_matrix[index_vertex2 - 1, index_vertex1 - 1] = 1
                self.n_edges += 1

    def scale(self, height, weight):
        """
        :param height: int con la altura de la pintura en cm
        :param weight: ...
        """
        x_max, y_max = max(self.points)
        if x_max > height or y_max > weight:
            raise NotImplementedError
        x_scale, y_scale = weight/x_max, height/y_max
        for i, point in enumerate(self.points):
            self.points[i] = (point[0] * x_scale, point[1] * y_scale)

    def make_graph(self):
        self.dist_matrix = np.zeros((len(self.points), len(self.points)), dtype=np.float64)
        for i in range(len(self.points)):
            for j in range(len(self.points)):
                if self.connection_matrix[i, j] == 1:
                    self.dist_matrix[i, j] = distance.euclidean(self.points[i], self.points[j])
        scipy_matrix = csr_matrix(self.dist_matrix)
        self.dist_matrix[self.dist_matrix == 0.0] = np.inf
        self.dist_matrix2, self.predec_matrix = csgraph.floyd_warshall(scipy_matrix,
                                                                       directed=False,
                                                                       return_predecessors=True)

    def go_to_init(self):
        for _ in range(self.N_BOTS):
            obj_with_movement = Movement(self.MODE, self._OVERRIDED_FUNCTIONS)
            self.objs_with_movement.append(obj_with_movement)
        if self.N_BOTS == 1:
            initial_points = [np.random.choice(self.points)]
        if self.N_BOTS >= 2:
            shape = self.connection_matrix.shape
            indexes_pair = np.unravel_index(np.argmax(self.dist_matrix2), shape)
            initial_points = [self.points[indexes_pair[0]], self.points[indexes_pair[1]]]
            if self.N_BOTS > 2:
                for _ in range(self.N_BOTS - 2):
                    indexes_list = list(indexes_pair)
                    big_col = np.sum(self.dist_matrix2[:, indexes_list], axis=1)  # consiguiendo
                    # las columnas de los puntos más distantes i suma sus filas
                    new_index = np.argmax(big_col)
                    indexes_list.append(new_index)
                initial_points = [self.points[i] for i in indexes_list]
        for i, obj_with_movement in enumerate(self.objs_with_movement):
            obj_with_movement.penup()
            obj_with_movement.setpos(initial_points[i][0], initial_points[i][1])
            obj_with_movement.pendown()
        return initial_points

    def paint(self, initial_points):
        def move(index_point1, index_point2):
            p1 = self.points[index_point1]
            p2 = self.points[index_point2]
            x, y = p2[0] - p1[0], p2[1] - p1[1]
            angle = math.atan2(y, x)
            angle = math.degrees(angle % (2 * math.pi))
            angle2 = angle - obj_with_movement.angle
            if angle2 <= 180:
                obj_with_movement.left(angle2)
            else:
                obj_with_movement.right(360 - angle2)
            obj_with_movement.angle = angle
            distance_bet_p = distance.euclidean(p1, p2)
            obj_with_movement.forward(distance_bet_p)
        indexes_initial_points = [self.points.index(point) for point in initial_points]
        painted_matrix = np.full(self.connection_matrix.shape, np.nan)
        painted_matrix[self.connection_matrix == 1] = 0.0
        for initial_point in indexes_initial_points:
            painted_matrix[initial_point, :] += 2.0
        pos_objs = indexes_initial_points
        n_edges = 0
        while True:
            for i, obj_with_movement in enumerate(self.objs_with_movement):
                if n_edges == self.n_edges:
                    return
                actual_point = pos_objs[i]
                distances = self.dist_matrix[:, actual_point]
                distances[painted_matrix[:, actual_point] > 0] = np.inf
                if (painted_matrix[:, actual_point] == 0.0).any():
                    nearest_not_painted_point = np.argmin(distances)
                    move(actual_point, nearest_not_painted_point)
                    painted_matrix[actual_point, :] -= 2.0
                    painted_matrix[nearest_not_painted_point, :] += 2.0
                    painted_matrix[nearest_not_painted_point, actual_point] += 1.0
                    painted_matrix[actual_point, nearest_not_painted_point] += 1.0
                    n_edges += 1
                    pos_objs[i] = nearest_not_painted_point
                else:
                    distances = self.dist_matrix2[:, actual_point]
                    distances[painted_matrix[:, actual_point] > 1.0] = np.inf
                    indexes = np.argsort(distances)
                    for i2 in indexes:
                        if (painted_matrix[:, i2] == 0.0).any():
                            nearest_free_point = i2
                            break
                    obj_with_movement.penup()
                    while actual_point != nearest_free_point:
                        next_point = self.predec_matrix[actual_point, nearest_free_point]
                        move(actual_point, next_point)
                        actual_point = next_point
                    obj_with_movement.pendown()
                    painted_matrix[actual_point, :] -= 2.0
                    painted_matrix[nearest_free_point, :] += 2.0
                    pos_objs = nearest_free_point

    def run(self, height, weight, filenames=None):
        if not self.is_filename_in_constr and filenames is not None:
            self.read_paint(filenames[0], filenames[1])
        self.scale(height, weight)
        self.make_graph()
        points = self.go_to_init()
        self.paint(points)
        turtle.mainloop()
