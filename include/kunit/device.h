/* SPDX-License-Identifier: GPL-2.0 */
/*
 * KUnit basic device implementation
 *
 * Helpers for creating and managing fake devices for KUnit tests.
 *
 * Copyright (C) 2023, Google LLC.
 * Author: David Gow <davidgow@google.com>
 */

#ifndef _KUNIT_DEVICE_H
#define _KUNIT_DEVICE_H

#if IS_ENABLED(CONFIG_KUNIT)

#include <kunit/test.h>

struct device;
struct device_driver;

/**
 * kunit_driver_create() - Create a struct device_driver attached to the kunit_bus
 * @test: The test context object.
 * @name: The name to give the created driver.
 *
 * Creates a struct device_driver attached to the kunit_bus, with the name @name.
 * This driver will automatically be cleaned up on test exit.
 *
 * Return: a stub struct device_driver, managed by KUnit, with the name @name.
 */
struct device_driver *kunit_driver_create(struct kunit *test, const char *name);

/**
 * kunit_device_register() - Create a struct device for use in KUnit tests
 * @test: The test context object.
 * @name: The name to give the created device.
 *
 * Creates a struct kunit_device (which is a struct device) with the given name,
 * and a corresponding driver. The device and driver will be cleaned up on test
 * exit, or when kunit_device_unregister is called.
 *
 * Return: a pointer to a struct device which will be cleaned up when the test
 * exits, or an error pointer if the device could not be allocated or registered.
 */
struct device *kunit_device_register(struct kunit *test, const char *name);

/**
 * kunit_device_unregister() - Unregister a KUnit-managed device
 * @test: The test context object which created the device
 * @dev: The device.
 *
 * Unregisters and destroys a struct device which was created with
 * kunit_device_register. The created driver is also cleaned up.
 */
void kunit_device_unregister(struct kunit *test, struct device *dev);

#endif

#endif
