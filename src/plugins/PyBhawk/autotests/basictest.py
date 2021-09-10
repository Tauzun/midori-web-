import Bhawk
import unittest

class BasicTest(unittest.TestCase):

    def test_module_version(self):
        self.assertEqual(Bhawk.__version__.count('.'), 2)
        self.assertIsNotNone(Bhawk.registerPlugin)

    def test_mainapplication(self):
        self.assertIsNotNone(Bhawk.MainApplication.instance())

    def test_create_window(self):
        window = Bhawk.MainApplication.instance().createWindow(Bhawk.Qz.BW_NewWindow)
        self.assertIsNotNone(window)


suite = unittest.defaultTestLoader.loadTestsFromTestCase(BasicTest)
if unittest.TextTestRunner().run(suite).failures:
    raise(Exception("FAIL"))
