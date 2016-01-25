<?php

/**
 * Show the Custom Entity page.
 */
function custom_entity_page_view($custom_entity, $view_mode = 'full') {
  return $custom_entity->view($view_mode, NULL, TRUE);
}


/**
 * Returns a custom title
 */
function custom_entity_page_title($custom_entity) {
  return "$custom_entity->label ($custom_entity->id)";
}

/**
 * Form defintion for the custom_entity forms
 */
function custom_entity_form($form, &$form_state, $custom_entity) {
  global $user;

  $form['uid'] = array(
    '#value' => $user->uid,
  );
  $form['#id'] = 'custom_entity-form';

  // store the custom_entity for later
  $form['#custom_entity'] = $custom_entity;
  $form_state['custom_entity'] = $custom_entity;

  // label field
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom Entity Name'),
    '#default_value' => isset($custom_entity->label) ? $custom_entity->label : '',
    '#weight' => -5,
    '#required' => TRUE,
    '#access' => user_access('edit custom_entity'),
  );

  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($custom_entity->label) ? $custom_entity->name : '',
    '#maxlength' => 255,
    '#description' => t('A unique name for competitions. It must only contain lowercase letters, numbers and hyphens.'),
    '#machine_name' => array(
      'source' => array('label'),
      'exists' => 'custom_entity_name_exists',
      'replace_pattern' => '[^a-z0-9-]+',
      'replace' => '-',
    ),
    '#element_validate' => array('form_validate_machine_name'),
    '#weight' => -10,
    '#access' => user_access('edit custom_entity'),
  );

  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  $form['path'] = its_core_create_path_field($custom_entity);

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Publishing Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
    '#weight' => 95,
    '#access' => user_access('edit custom_entity'),
  );

  $form['options']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published'),
    '#default_value' => isset($custom_entity->status) ? $custom_entity->status : 0,
  );

  // buttons
  $form['buttons'] = array();
  $form['buttons']['#weight'] = 100;
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
    '#submit' => array('custom_entity_form_submit'),
  );

  if (!empty($custom_entity->id)) {
    $form['buttons']['delete'] = array(
      '#access' => user_access('delete custom_entity'),
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 15,
      '#submit' => array('custom_entity_form_delete_submit'),
    );
  }

  field_attach_form('custom_entity', $custom_entity, $form, $form_state);

  return $form;
}

/**
 * Machine name callback to make sure it name does not already exist.
 */
function custom_entity_name_exists($value) {
  return db_query_range('SELECT 1 FROM {custom_entity} WHERE name = :name', 0, 1, array(':name' => $value))->fetchField();
}

/**
 * Validate hook for custom_entity forms
 */
function custom_entity_form_validate($form, &$form_state) {
  $custom_entity = $form_state['custom_entity'];
  field_attach_form_validate('custom_entity', $custom_entity, $form, $form_state);
}

/**
 * Implements custom_entity_form_submit
 */
function custom_entity_form_submit($form, &$form_state) {
  if (user_access('edit custom_entity')) {
    $form_state['values']['uid'] = $form['uid']['#value'];
  }
  $custom_entity = &$form_state['custom_entity'];

  entity_form_submit_build_entity('custom_entity', $custom_entity, $form, $form_state);
  $custom_entity->save();
  $form_state['redirect'] = 'custom_entity/' . $custom_entity->id;
}

/**
 * Page callback. Edit a Competition.
 */
function custom_entity_edit($custom_entity) {
  drupal_set_title(t('Edit Custom Entity: @title', array('@title' => $custom_entity->label)));

  return drupal_get_form('custom_entity_form', $custom_entity);
}

function custom_entity_confirm_delete_page($form, &$form_state, $custom_entity) {
  $form = array();

  $form["#custom_entity"] = $custom_entity;
  return confirm_form(
    $form,
    t('Are you sure you want to delete the custom_entity %title?', array('%title' => $custom_entity->label)),
    "custom_entity/$custom_entity->id",
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel'),
    'custom_entity_confirm_delete_page_submit'
  );
}

/**
 * Delete submit callback.
 */
function custom_entity_confirm_delete_page_submit($form, &$form_state) {
  $form['#custom_entity']->delete();
  $form_state['redirect'] = "admin/content/custom_entity";
}

/**
 * Submit handler for delete button
 */
function custom_entity_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'custom_entity/' . $form_state['custom_entity']->id. '/delete';
}