<?php

/**
 * Developer tool to generate dummy votes.
 */
function custom_entity_generate_form($form, $form_state, $name) {
  $form['entity_label'] = array(
    '#type' => 'value',
    '#value' => $name,
  );
  $form['kill_entities'] = array(
    '#type' => 'checkbox',
    '#title' => t('<strong>Delete all @name</strong> before generating new @name.', ['@name' => $name]),
    '#default_value' => FALSE,
  );

  $form['num_entities'] = array(
    '#type' => 'textfield',
    '#title' => t('How many @name would you like to generate?', ['@name' => $name]),
    '#default_value' => 50,
    '#size' => 10,
  );

  $form['title_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Max word length of titles'),
    '#default_value' => 4,
    '#size' => 10,
  );

  unset($options);
  $options[LANGUAGE_NONE] = t('Language neutral');
  if (module_exists('locale')) {
    $options += locale_language_list();
  }
  $form['add_language'] = array(
    '#type' => 'select',
    '#title' => t('Set language on @name', ['@name' => $name]),
    '#multiple' => TRUE,
    '#disabled' => !module_exists('locale'),
    '#description' => t('Requires locale.module'),
    '#options' => $options,
    '#default_value' => array(LANGUAGE_NONE => LANGUAGE_NONE),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate'),
  );
  $form['#redirect'] = FALSE;

  return $form;
}

/**
 * Submit handler for generate form.
 */
function custom_entity_generate_form_submit($form_id, &$form_state) {
  if ($form_state['values']['num_entities'] <= 50) {
    custom_entity_generate_entities($form_state);
  }
  else {
    custom_entity_generate_batch_entities($form_state);
  }
}

/**
 * Pre-process the devel_generate_content_form request.  This is needed so
 * batch api can get the list of users once.
 *
 * @param $num
 *   array of options obtained from devel_generate_products_form.
 */
function custom_entity_generate_entities_pre_entity(&$results) {
  module_load_include('inc', 'devel_generate');
  // Get user id.
  $users = devel_get_users();
  $users = array_merge($users, array('0'));
  $results['users'] = $users;
}

function custom_entity_generate_entities($form_state) {
  if (!empty($form_state['values']['kill_entities'])) {
    custom_entity_generate_entities_kill($form_state['values']);
  }

  custom_entity_generate_entities_pre_entity($form_state['values']);
  $start = time();
  for ($i = 1; $i <= $form_state['values']['num_entities']; $i++) {
    custom_entity_generate_custom_entity_add_custom_entity($form_state['values']);

    if (function_exists('drush_log') && $i % drush_get_option('feedback', 1000) == 0) {
      $now = time();
      drush_log(dt('Completed !feedback entities (!rate entities/min)', array('!feedback' => drush_get_option('feedback', 1000), '!rate' => (drush_get_option('feedback', 1000)*60)/($now-$start))), 'ok');
      $start = $now;
    }
  }

  devel_generate_set_message(format_plural($form_state['values']['num_entities'], '1 entity created.', 'Finished creating @count entities'));
}

function custom_entity_generate_entities_kill($values) {
  $table = 'custom_entity';

  $results = db_select($table, 'p')
    ->fields('p', array('id'))
//    ->condition('type', $values['custom_entity_types'], 'IN')
    ->execute();
  foreach ($results as $result) {
    $entity_ids[] = $result->id;
  }

  if (!empty($entity_ids)) {
    custom_entity_delete_multiple($entity_ids);
    drupal_set_message(t('Deleted %count entities.', array('%count' => count($entity_ids))));
  }
}

function custom_entity_generate_custom_entity_add_custom_entity(&$results) {
  $entity = entity_create('custom_entity', ['type' => 'custom_entity']);
  $entity->label = devel_create_greeking(mt_rand(2, $results['title_length']), TRUE);
  $entity->name = strtolower(str_replace(' ', '-',$entity->label));
  $entity->uid = 1;
  $entity->status = (bool) rand(0,4);

  devel_generate_set_language($results, $entity);

  // A flag to let hook_custom_entityapi() implementations know that this is a generated custom_entity.
  $entity->custom_entity_generate = $results;

  module_load_include('inc', 'devel_generate', 'devel_generate.fields');
  devel_generate_fields($entity, 'custom_entity', $entity->type);

  $entity->save();
}
