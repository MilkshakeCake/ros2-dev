import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import time

class HexagonDriver(Node):
    def __init__(self):
        super().__init__('hexagon_driver')
        self.publisher_ = self.create_publisher(Twist, '/turtle1/cmd_vel', 10)
        self.drive_hexagon()

    def drive_hexagon(self):
        msg = Twist()
        # A hexagon has 6 sides. For each side: move forward, then turn 60 degrees.
        for _ in range(6):
            # Move Forward
            msg.linear.x = 2.0
            msg.angular.z = 0.0
            self.publisher_.publish(msg)
            time.sleep(2.0) # Drive for 2 seconds

            # Stop and Turn (60 degrees = 1.047 radians)
            msg.linear.x = 0.0
            msg.angular.z = 1.047 
            self.publisher_.publish(msg)
            time.sleep(1.0) # Turn for 1 second

        # Stop at the end
        self.publisher_.publish(Twist())

def main(args=None):
    rclpy.init(args=args)
    node = HexagonDriver()
    rclpy.shutdown()

if __name__ == '__main__':
    main()