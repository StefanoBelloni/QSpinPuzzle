from PySide6 import QtCore, QtWidgets, QtGui
import copy
import math

import spyn_puzzle as spyn

class QSpinPuzzleWidget(QtWidgets.QWidget):

    def __init__(self, size, parent) -> None:
        super().__init__(parent)
        # =============== #
        self._debug = False
        # =============== #
        self.reset_btn = QtWidgets.QPushButton("reset", self)
        self.reset_btn.clicked.connect(self.reset)
        # =============== #
        self.shuffle_btn = QtWidgets.QPushButton("shuffle", self)
        self.shuffle_btn.clicked.connect(self.shuffle)
        # =============== #
        self.twist_btn = QtWidgets.QPushButton("twist", self)
        self.twist_btn.clicked.connect(self.swap_side)
        # =============== #
        self.north_spin_btn = QtWidgets.QPushButton("spin", self)
        self.north_spin_btn.clicked.connect(self.spin_north)
        # =============== #
        self.east_spin_btn = QtWidgets.QPushButton("spin", self)
        self.east_spin_btn.clicked.connect(self.spin_east)
        # =============== #
        self.west_spin_btn = QtWidgets.QPushButton("spin", self)
        self.west_spin_btn.clicked.connect(self.spin_west)

        # =============== #
        self._game = spyn.SpinPuzzleGame()
        self.set_size(size)
        self._lastPositionMause = None
        # =============== #
        self.setFocus()
        # =============== #

    def _create_polygon(self):
        L = self._length
        R = L / 4
        center = QtCore.QPoint(L / 2, (2 * math.sqrt(3) + 3) * L / 12)
        centerTop = QtCore.QPoint(L / 2, L / 4)
        topCorner = QtCore.QPoint(0, R)
        arc_steps = 100
        angle = 30
        _polygon = QtGui.QPolygon()
        _polygon.append(centerTop)
        for i in range(0, arc_steps + 1):
            _polygon.append(QtCore.QPoint(
                topCorner.x() + R * math.cos(math.radians(i * angle / arc_steps)), 
                topCorner.y() + R * math.sin(math.radians(i * angle / arc_steps)))
                )
        _polygon.append(center)

        rightCorner = QtCore.QPoint(L, L / 4);
        for i in range(arc_steps, -1, -1):
            _polygon.append(QtCore.QPoint(
                rightCorner.x() - R * math.cos(math.radians(i * angle / arc_steps)),
                rightCorner.y() - R * math.sin(math.radians(-i * angle / arc_steps)))
            )
        return _polygon

    def set_size(self, size):
        self._length = size
        self._width = 15
        r = self._get_radius_internal() - self._width
        L = self._length;
        self._radius_marble = r * math.sin(math.pi / 5) / 2
        self._polygon = self._create_polygon()
        self.shuffle_btn.setGeometry(0, 4 * self._length / 24, 3 * self._length / 24, self._length / 24)
        self.twist_btn.setGeometry(0, 5 * self._length / 24, 3 * self._length / 24, self._length / 24)
        self.north_spin_btn.setGeometry(L / 2 - L / 24, 0, 3 * L / 24, L / 24)
        self.reset_btn.setGeometry(0, 3 * self._length / 24, 3 * self._length / 24, self._length / 24)
        self.west_spin_btn.setGeometry(0, L / 2, 3 * L / 24, L / 24)
        self.east_spin_btn.setGeometry(L - 3 * L / 24, L / 2, 3 * L / 24, L / 24)
    
    def reset(self):
        self._game.reset()
        self.update()
        # print(self._game)

    def shuffle(self):
        self._game.shuffle()
        self.update()
        # print(self._game)

    def swap_side(self):
        self._game.swap_side()
        self.update()
        # print(self._game)
    
    def spin_north(self):
        self._game.spin_leaf(spyn.LEAF.NORTH)
        self.update()
        # print(self._game)

    def spin_east(self):
        self._game.spin_leaf(spyn.LEAF.EAST)
        self.update()
        # print(self._game)

    def spin_west(self):
        self._game.spin_leaf(spyn.LEAF.WEST)
        self.update()
        # print(self._game)
    
    def _get_radius_internal(self):
        return 0.185 * self._length

    def _paint_puzzle_section(self, painter, color, color_internal):
        painter.drawPolygon(self._polygon)
        L = self._length;
        R = L / 4;
        centerTop = QtCore.QPoint(L / 2, L / 4);
        internalRadius = self._get_radius_internal();
        # primary = m_game.get_active_side() == puzzle::SIDE::FRONT;

        painter.setBrush(color);
        # The span angle for an ellipse segment to angle , which is in 16ths of a
        # degree 360 * 16 / 2 is half the full ellipse
        painter.drawPie(R, 0, 2 * R, 2 * R, 0, 360 * 8);
        painter.setBrush(color_internal);
        painter.drawPolygon(self._polygon);

        painter.setBrush(color);
        painter.drawEllipse(centerTop, internalRadius, internalRadius);
        painter.drawLine(L / 2 - R, L / 4, L / 2 + R, L / 4);
    
    def _next_section(self, painter, angle=120):
        L = self._length;
        center = QtCore.QPoint(L / 2, (2 * math.sqrt(3) + 3) * L / 12);
        painter.translate(center);
        painter.rotate(angle);
        painter.translate(-center);

    def _paint_internal_circular_guide(self, painter, color):
        L = self._length
        r = self._get_radius_internal() - self._width / 2
        center = QtCore.QPoint(L / 2, (2 * math.sqrt(3) + 3) * L / 12)

        pen = QtGui.QPen()
        pen.setBrush(color)
        if (self._width > 0): pen.setWidth(self._width)
        painter.setPen(pen)
        angle1 = 90 + 36 * 4
        angle2 = 36 * 2
        painter.drawLine(center - QtCore.QPoint(L / 24, 0), center + QtCore.QPoint(L / 24, 0))
        painter.drawArc(L / 2 - r, L / 4 - r, 2 * r, 2 * r, angle1 * 16, angle2 * 16)
    
    def _paint_marbles(self, painter):
        if self._game.get_side().trifoild_status() != spyn.TREFOIL.BORDER_ROTATION:
            self._do_paint_marbels(painter)
        else:
            # painter.restore();
            self._do_paint_marbles_on_border(painter)
    
    def _do_paint_marbles_on_border(self, painter):
        game_side = self._game.get_side();
        it = game_side.begin();
        L = self._length;
        r = self._get_radius_internal() - self._width;
        center = QtCore.QPoint(L / 2, L / 4);
        shift_local = math.radians(it.get_angle() * 12);
        internal_dist_angle = self._game.get_phase_shift_internal_disk();

        # NORTH
        for n in range(0, 7):
            self._paint_marble(shift_local, painter, it, center, n + 2, r);
            it += 1
        self._next_section(painter, internal_dist_angle);
        for n in range(9, 6, -1):
            self._paint_marble(-shift_local, painter, it, center, n + 2, r);
            it += 1
        # EAST
        self._next_section(painter, 120 - internal_dist_angle);
        for n in range(0, 7):
            self._paint_marble(shift_local, painter, it, center, n + 2, r);
            it += 1
        self._next_section(painter, internal_dist_angle);
        for n in range(9, 6, -1):
            self._paint_marble(-shift_local, painter, it, center, n + 2, r);
            it += 1
        # WEST
        self._next_section(painter, 120 - internal_dist_angle);
        for n in range(0, 7):
            self._paint_marble(shift_local, painter, it, center, n + 2, r);
            it += 1
        self._next_section(painter, internal_dist_angle);
        for n in range(9, 6, -1):
            self._paint_marble(-shift_local, painter, it, center, n + 2, r);
            it += 1
    
    def _do_paint_marbels(self, painter):
        painter.save()
        self._paint_marbles_on_leaf(painter, spyn.LEAF.NORTH)
        self._next_section(painter)
        self._paint_marbles_on_leaf(painter, spyn.LEAF.EAST)
        self._next_section(painter)
        self._paint_marbles_on_leaf(painter, spyn.LEAF.WEST)
        painter.restore()
        for phase, leaf in [
            (self._game.get_phase_shift_internal_disk(), spyn.LEAF.NORTH),
            (120.0, spyn.LEAF.EAST),
            (120.0, spyn.LEAF.WEST)
        ]:
            self._next_section(painter, phase)
            self._paint_marbles_on_internal_circle(painter, leaf)
    
    def _paint_marbles_on_internal_circle(self, painter, leaf):
        game_side = self._game.get_side();
        L = self._length;
        r = self._get_radius_internal() - self._width;
        center = QtCore.QPoint(L / 2, L / 4);
        it = game_side.begin(leaf);
        it += 7
        shift_local = math.radians(it.get_angle());
        start = 0;
        end = 3;
        increment = 1;

        for n in range(start, end, increment):
            self._paint_marble(shift_local, painter, it, center, n + 9, r);
            it += 1
    
    def _paint_marbles_on_leaf(self, painter, leaf):
        game_side = self._game.get_side();
        L = self._length;
        r = self._get_radius_internal() - self._width;
        center = QtCore.QPoint(L / 2, L / 4);
        it = game_side.begin(leaf);
        shift_local = math.radians(it.get_angle());
        if game_side.trifoild_status() == spyn.TREFOIL.BORDER_ROTATION:
            shift_local = math.radians(it.get_angle() * 12.0);

        for n in range(0, 7):
            self._paint_marble(shift_local, painter, it, center, n + 2, r)
            it += 1

    def _paint_marble(self, shift_local, painter, it, center, n, r):
        painter.setBrush(self.toQtColor(it.color()));
        position = center + QtCore.QPoint(r * math.cos(shift_local + math.pi / 2 + n * math.pi / 5),
                            r * math.sin(shift_local + math.pi / 2 + n * math.pi / 5));
        painter.drawEllipse(position, self._radius_marble, self._radius_marble);
        if self._debug: 
            painter.drawText(position, str(it.id()));

    def _paint_status(self):
        L = self._length;
        side = self._game.get_side();

        painter_status = QtGui.QPainter(self)

        try:
            if (side.trifoild_status() == spyn.TREFOIL.BORDER_ROTATION or
                side.is_border_rotation_possible()):
                painter_status.setBrush(self._green())
            elif (side.trifoild_status() == spyn.TREFOIL.INVALID):
                painter_status.setBrush(self._red())
            else:
                painter_status.setBrush(self._yellow());
            painter_status.drawRect(0, 0, L / 24, L / 24);

            painter_status.setBrush( self._green() if (side.is_rotation_possible(spyn.LEAF.NORTH)) else self._red())
            painter_status.drawRect(0 + 0 * L / 24, L / 24, L / 24, L / 24)
            painter_status.setBrush( self._green() if (side.is_rotation_possible(spyn.LEAF.EAST)) else self._red())
            painter_status.drawRect(0 + 1 * L / 24, L / 24, L / 24, L / 24)
            painter_status.setBrush( self._green() if (side.is_rotation_possible(spyn.LEAF.WEST)) else self._red())
            painter_status.drawRect(0 + 2 * L / 24, L / 24, L / 24, L / 24)

            keyboard_status = "KB: "
            if (self._game.get_keybord_state() == spyn.LEAF.NORTH):
                keyboard_status += "NORTH";
            elif (self._game.get_keybord_state() == spyn.LEAF.EAST):
                keyboard_status += "EAST";
            elif (self._game.get_keybord_state() == spyn.LEAF.WEST):
                keyboard_status += "WEST";
            elif (self._game.get_keybord_state() == spyn.LEAF.INVALID):
                keyboard_status += "CENTER";
            painter_status.drawText(QtCore.QPoint(0, 3 * L / 24 * (1)), keyboard_status);
        finally:
            painter_status.end()

    def _can_rotate_internal(self): return True

    def _get_speed(self): return 1.5 / self._length

    def _mouse_event_inside_internal_circle(self, event):
        if not self._can_rotate_internal():
            return False;

        L = self._length;
        center = QtCore.QPoint(L / 2, (2 * math.sqrt(3) + 3) * L / 12);
        internalRadius = self._get_radius_internal() + self._width / 2;

        pos_center = copy.deepcopy(event.pos()) - center;
        if (pos_center.dotProduct(pos_center, pos_center) > internalRadius * internalRadius):
            return False;

        pos = copy.deepcopy(event.pos())
        last = copy.deepcopy(self._lastPositionMause)
        pos -= center;
        last -= center;
        angle_new = math.atan2(pos.y(), pos.x());
        angle_old = math.atan2(last.y(), last.x());

        if ((angle_new * angle_old < 0) and math.fabs(angle_new - angle_old) > math.pi):
            angle_new = math.fmod(angle_new + math.pi, 2 * math.pi)
            angle_old = math.fmod(angle_old + math.pi, 2 * math.pi)

        delta_alpha = math.copysign(1, angle_new - angle_old) * 360.0 * self._get_speed();
        self._lastPositionMause = event.pos();

        self._game.rotate_internal_disk(delta_alpha);
        return True;
    
    def _is_mouse_on_leaf_marbles(self, ev, center):
        internalRadius = self._get_radius_internal();

        pos = copy.deepcopy(ev.pos()) - center;
        r = math.sqrt(pos.dotProduct(pos, pos));
        if (r < internalRadius and r > internalRadius - 2 * self._radius_marble):
            return True
        return False;

    def _mouse_event_inside_leaf(self, ev, center, leaf):
        if not self._is_mouse_on_leaf_marbles(ev, center):
            return False;
        pos = copy.deepcopy(ev.pos())
        last = copy.deepcopy(self._lastPositionMause)
        pos -= center
        last -= center

        angle_new = math.atan2(pos.y(), pos.x());
        angle_old = math.atan2(last.y(), last.x());

        if ((angle_new * angle_old < 0) and math.fabs(angle_new - angle_old) > math.pi):
            angle_new = math.fmod(angle_new + math.pi, 2 * math.pi);
            angle_old = math.fmod(angle_old + math.pi, 2 * math.pi);

        delta_angle = math.copysign(1, angle_new - angle_old) * 360.0 * self._get_speed()
        side = self._game.get_side();
        side.rotate_marbles(leaf, delta_angle);
        self._lastPositionMause = ev.pos();

        return True;

    def _processKey(self, key, fraction_angle):
        return self._game.process_key(key, fraction_angle);

    # ========================================= #
    # EVENTS
    # ========================================= #
    def mousePressEvent(self, event):
        self._lastPositionMause = event.pos()
    
    def mouseMoveEvent(self, event):
        L = self._length
        center_north = QtCore.QPoint(L / 2, L / 4);
        center_west = QtCore.QPoint(L / 4, (math.sqrt(3) + 1) / 4 * L);
        center_east = QtCore.QPoint(3 * L / 4, (math.sqrt(3) + 1) / 4 * L);

        if self._mouse_event_inside_internal_circle(event):
            self.update();
        elif (
            self._mouse_event_inside_leaf(event, center_north, spyn.LEAF.NORTH) or
            self._mouse_event_inside_leaf(event, center_east, spyn.LEAF.EAST) or
            self._mouse_event_inside_leaf(event, center_west, spyn.LEAF.WEST)):
            self.update();

    def paintEvent(self,event):
        self._paint_status()
        painter = QtGui.QPainter(self)
        L = self._length
        center = QtCore.QPoint(L / 2, (2 * math.sqrt(3) + 3) * L / 12)
        primary = self._game.get_active_side() == spyn.SIDE.FRONT
        if primary:
            colorNorth = self._red()
            colorEast = self._blue()
            colorWest = self._yellow()
            colorInternal = self._magenta()
        else:
            colorNorth = self._green()
            colorEast = self._darkBlue()
            colorWest = self._darkYellow()
            colorInternal = self._magenta()

        painter.save()
        try:
            # ================================================================ #
            self._paint_puzzle_section(painter, colorNorth, colorInternal)
            self._next_section(painter)
            self._paint_puzzle_section(painter, colorEast, colorInternal)
            self._next_section(painter)
            self._paint_puzzle_section(painter, colorWest, colorInternal)
            # ================================================================ #
            painter.restore()
            painter.save()
            # --------------- #
            internalRadius = self._get_radius_internal() + self._width / 2;
            painter.setBrush(QtGui.QColor(0, 125, 125));
            painter.drawEllipse(center, internalRadius, internalRadius);
            # ================================================================ #
            painter.restore()
            painter.save()
            # --------------- #
            self._next_section(painter, self._game.get_phase_shift_internal_disk())
            self._paint_internal_circular_guide(painter, self._darkBlue())
            self._next_section(painter)
            self._paint_internal_circular_guide(painter, self._darkBlue())
            self._next_section(painter)
            self._paint_internal_circular_guide(painter, self._darkBlue())
            # ================================================================ #
            painter.restore()
            # ================================================================ #
            self._paint_marbles(painter)

        finally:
            painter.end()

    def keyPressEvent(self, e):
        if (self._processKey(e.key(), 1.0 / 4.0)):
            self.update();
            self.setFocus();

    def resizeEvent(self, e):
        length = min(e.size().width(), e.size().height())
        self.set_size(length)
        self.update()
    # ========================================= #
    # COLORS
    # ========================================= #
    def toQtColor(self, color):
        if color ==  spyn.white:
            return self._white()
        if color ==  spyn.black:
            return self._black()
        if color ==  spyn.red:
            return self._red()
        if color ==  spyn.darkRed:
            return self._darkRed()
        if color ==  spyn.green:
            return self._green()
        if color ==  spyn.darkGreen:
            return self._darkGreen()
        if color ==  spyn.blue:
            return self._blue()
        if color ==  spyn.darkBlue:
            return self._darkBlue()
        if color ==  spyn.cyan:
            return self._cyan()
        if color ==  spyn.darkCyan:
            return self._darkCyan()
        if color ==  spyn.magenta:
            return self._magenta()
        if color ==  spyn.darkMagenta:
            return self._darkMagenta()
        if color ==  spyn.yellow:
            return self._yellow()
        if color ==  spyn.darkYellow:
            return self._darkYellow()
        if color ==  spyn.gray:
            return self._gray()
        if color ==  spyn.darkGray:
            return self._darkGray()
        if color ==  spyn.lightGray():
            return self._lightGray()
        return self._black()

    def _white(self): return QtGui.QColor(0xFF, 0xFF, 0xFF)
    def _black(self): return QtGui.QColor(0, 0, 0)
    def _red(self): return QtGui.QColor(0xff, 0x00, 0x00)
    def _darkRed(self): return QtGui.QColor(0x80, 0x00, 0x00)
    def _green(self): return QtGui.QColor(0x00, 0xff, 0x00)
    def _darkGreen(self): return QtGui.QColor(0x00, 0x80, 0x00)
    def _blue(self): return QtGui.QColor(0x00, 0x00, 0xff)
    def _darkBlue(self): return QtGui.QColor(0x00, 0x00, 0x80)
    def _cyan(self): return QtGui.QColor(0x00, 0xff, 0xff)
    def _darkCyan(self): return QtGui.QColor(0x00, 0x80, 0x80)
    def _magenta(self): return QtGui.QColor(0xff, 0x00, 0xff)
    def _darkMagenta(self): return QtGui.QColor(0x80, 0x00, 0x80)
    def _yellow(self): return QtGui.QColor(0xff, 0xff, 0x00)
    def _darkYellow(self): return QtGui.QColor(0x80, 0x80, 0x00)
    def _gray(self): return QtGui.QColor(0xa0, 0xa0, 0xa4)
    def _darkGray(self): return QtGui.QColor(0x80, 0x80, 0x80)
    def _lightGray(self): return QtGui.QColor(0xc0, 0xc0, 0xc0)
