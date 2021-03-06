#! /usr/bin/env python3

import config

config.import_libs()

import unittest
import json
import time

from rest import GET, POST, DELETE, PUT

class TestDevicesUpdateActivate(unittest.TestCase):
	""" 
	Create a session for testing.
	"""
	def setUp(self):
		req = POST(config.ui_host, config.ui_port, "/auth")
		req.body(config.PERMIT_LOGIN)
		response, content = req()

		self.assertEqual(200, response.status)

		result = json.loads(content)
		self.assertEqual("success", result["status"])
		self.session = result["data"]["id"]

		req = POST(config.ui_host, config.ui_port, "/gateways")
		req.authorize(self.session)
		req.body(json.dumps(
			{"id": config.gateway_id, "name": "Testing Gateway", "timezone_id": "Europe/Prague"}
		))
		response, _ = req()

		self.assertEqual(201, response.status)

	def tearDown(self):
		req = DELETE(config.ui_host, config.ui_port, "/gateways/" + config.gateway_id)
		req.authorize(self.session)
		response, content = req()

		self.assertEqual(204, response.status)

		req = DELETE(config.ui_host, config.ui_port, "/auth")
		req.authorize(self.session)
		response, content = req()

		self.assertEqual(204, response.status)

	"""
	Update a non-existing device should fail.
	"""
	def test1_update_non_existing(self):
		req = PUT(config.ui_host, config.ui_port, "/gateways/" + config.gateway_id + "/devices/0x413")
		req.authorize(self.session)
		req.body(json.dumps({
			"name": "New device name"
		}))

		response, content = req()
		self.assertEqual(400, response.status)

	"""
	Change name of the device 4931887558509748622 (Unknown).
	"""
	def test2_update_existing(self):
		req = PUT(config.ui_host, config.ui_port, "/gateways/" + config.gateway_id + "/devices/0xa37e0f7f0302324d")
		req.authorize(self.session)
		req.body(json.dumps({
			"name": "New device name"
		}))

		response, content = req()
		self.assertEqual(200, response.status)
		result = json.loads(content)
		self.assertEqual("New device name", result["data"].get("name"))

	"""
	Attempt to unpair a device should fail on permissions.
	"""
	def test3_unpair_non_existing(self):
		req = DELETE(config.ui_host, config.ui_port, "/gateways/" + config.gateway_id + "/devices/0xf101201230234023")
		req.authorize(self.session)
		response, content = req()

		self.assertEqual(403, response.status)
		result = json.loads(content)
		self.assertEqual("error", result["status"])
		self.assertEqual("not enough permission to access the resource", result["message"])

	"""
	Attempt to unpair the inactive device 4931887558509748622 (Unknown) should succeed.
	"""
	def test4_unpair_inactive(self):
		req = DELETE(config.ui_host, config.ui_port, "/gateways/" + config.gateway_id + "/devices/0xa371959aad24618e")
		req.authorize(self.session)
		response, content = req()

		self.assertEqual(202, response.status)

		data = json.loads(content)
		self.assertEqual("success", data["status"])

		device_uri = response.getheader("Location")

		for i in range(10):
			req = GET(config.ui_host, config.ui_port, device_uri)
			req.authorize(self.session)
			response, content = req()

			self.assertEqual(200, response.status)
			result = json.loads(content)
			self.assertEqual("success", result["status"])

			if result["data"]["state"] == "inactive":
				break

			time.sleep(1)

		req = GET(config.ui_host, config.ui_port, device_uri)
		req.authorize(self.session)
		response, content = req()

		self.assertEqual(200, response.status)
		result = json.loads(content)
		self.assertEqual("success", result["status"])

		self.assertEqual("inactive", result["data"]["state"])

	"""
	Activate device 4931887558509748622 (Unknown).
	"""
	def test5_activate_and_unpair(self):
		req = PUT(config.ui_host, config.ui_port, "/gateways/" + config.gateway_id + "/devices/0xa371959aad24618e")
		req.authorize(self.session)
		req.body(json.dumps({
			"name": "Activated device"
		}))

		response, content = req()
		result = json.loads(content)
		self.assertEqual(200, response.status)
		self.assertTrue("state" in result["data"])

		while result["data"]["state"] != "active":
			req = GET(config.ui_host, config.ui_port,
				"/gateways/" + config.gateway_id + "/devices/0xa371959aad24618e")
			req.authorize(self.session)
			response, content = req()

			result = json.loads(content)
			self.assertEqual(200, response.status)

		self.assertTrue("active_since" in result["data"])
		self.assertEqual("active", result["data"]["state"])

		req = DELETE(config.ui_host, config.ui_port, "/gateways/" + config.gateway_id + "/devices/0xa371959aad24618e")
		req.authorize(self.session)
		response, content = req()

		self.assertEqual(202, response.status)

		data = json.loads(content)
		self.assertEqual("success", data["status"])

		device_uri = response.getheader("Location")

		for i in range(10):
			req = GET(config.ui_host, config.ui_port, device_uri)
			req.authorize(self.session)
			response, content = req()

			self.assertEqual(200, response.status)
			result = json.loads(content)
			self.assertEqual("success", result["status"])

			if result["data"]["state"] == "inactive":
				break

			time.sleep(1)

		req = GET(config.ui_host, config.ui_port, device_uri)
		req.authorize(self.session)
		response, content = req()

		self.assertEqual(200, response.status)
		result = json.loads(content)
		self.assertEqual("success", result["status"])

		self.assertEqual("inactive", result["data"]["state"])

	"""
	The Temperature sensor has non-updatable refresh time (-1).
	Thus, we must be unable to change it to be updatable.
	"""
	def test6_update_non_updatable_refresh(self):
		req = PUT(config.ui_host, config.ui_port, "/gateways/" + config.gateway_id + "/devices/0xa335d00019f5234e")
		req.authorize(self.session)
		req.body(json.dumps({
			"refresh_time": 10
		}))

		response, content = req()
		result = json.loads(content)
		self.assertEqual(400, response.status)
		self.assertEqual(400, result["code"])
		self.assertEqual("invalid input data", result["message"])

		# leave it non-updatable should work
		req = PUT(config.ui_host, config.ui_port, "/gateways/" + config.gateway_id + "/devices/0xa335d00019f5234e")
		req.authorize(self.session)
		req.body(json.dumps({
			"refresh_time": -1
		}))

		response, content = req()
		result = json.loads(content)
		self.assertEqual(200, response.status)
		self.assertEqual("success", result["status"])
		self.assertNotIn("refresh_time", result["data"])

if __name__ == '__main__':
	import sys
	import taprunner
	unittest.main(testRunner=taprunner.TAPTestRunner(stream = sys.stdout))
